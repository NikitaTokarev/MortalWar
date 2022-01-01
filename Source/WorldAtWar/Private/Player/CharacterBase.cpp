// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CharacterBase.h"
#include "NaziZombie/Useables/WeaponBase.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	//RageModeCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("RageModeCamera"));
	//RageModeCamera->SetupAttachment(GetCapsuleComponent());
	//RageModeCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	//RageModeCamera->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	WeaponIndex = 0;
	bIsAiming = false;

	bCanChangeWeapon = true;
}






// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartingWeaponClass, SpawnParams);

		if (CurrentWeapon)
		{
			PreviousWeapon = CurrentWeapon;
			WeaponArray.Add(CurrentWeapon);
			CurrentWeapon->WeaponIsNowInHand(true);
			OnRep_AttachWeapon();
		}
		/*if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(SecondWeaponClass, SpawnParams))
		{
			Weapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_weaponSocket"));
			WeaponArray.Add(Weapon);			
		}*/
	}
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority() && CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, CurrentWeapon);
	//DOREPLIFETIME(ACharacterBase, PreviousWeapon);
	DOREPLIFETIME(ACharacterBase, WeaponArray);
	DOREPLIFETIME_CONDITION(ACharacterBase, bIsAiming, COND_SkipOwner);
	DOREPLIFETIME(ACharacterBase, bIsInfected);
	DOREPLIFETIME_CONDITION(ACharacterBase, bIsRage, COND_OwnerOnly);
}


// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind aiming events
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACharacterBase::OnAimingStart);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACharacterBase::OnAimingEnd);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::OnFire);

	DECLARE_DELEGATE_OneParam(FScrollWeaponSignature, bool /* true - Next, false - Privious */);
	PlayerInputComponent->BindAction<FScrollWeaponSignature>("SwitchNextWeapon", IE_Pressed, this, &ACharacterBase::SwitchNextWeapon, true);
	PlayerInputComponent->BindAction<FScrollWeaponSignature>("SwitchPreviousWeapon", IE_Pressed, this, &ACharacterBase::SwitchNextWeapon, false);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);
}



bool ACharacterBase::Server_SwitchWeapon_Validate(AWeaponBase* NewWeapon)
{
	return true;
}

void ACharacterBase::Server_SwitchWeapon_Implementation(AWeaponBase* NewWeapon)
{
	CurrentWeapon = NewWeapon;
	OnRep_AttachWeapon();

	if (!IsLocallyControlled())
	{
		Client_SwitchWeapon(NewWeapon);
	}
}

bool ACharacterBase::Client_SwitchWeapon_Validate(AWeaponBase* NewWeapon)
{
	return true;
}

void ACharacterBase::Client_SwitchWeapon_Implementation(AWeaponBase* NewWeapon)
{
	OnWeaponChanged.Broadcast();
}



void ACharacterBase::OnRep_AttachWeapon()
{
	if (PreviousWeapon)
	{
		PreviousWeapon->WeaponIsNowInHand(false);
		//PreviousWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	if (CurrentWeapon)
	{
		CurrentWeapon->WeaponIsNowInHand(true);

		if (WeaponArray.Num() > 1)
		{
			AWeaponBase* HiddenWeapon = CurrentWeapon == WeaponArray[0] ? WeaponArray[1] : WeaponArray[0];
			HiddenWeapon->WeaponIsNowInHand(false);
		}

		
		if (IsLocallyControlled())
		{
			CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_weaponSocket"));
			OnWeaponChanged.Broadcast();
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_weaponSocket"));
		}
	}	
	if (IsLocallyControlled())
	{
		PreviousWeapon = CurrentWeapon;
		/*if (WeaponArray.Num() > 1)
		{
			PreviousWeapon = CurrentWeapon == WeaponArray[0] ? WeaponArray[1] : WeaponArray[0];
		}
		else
		{
			PreviousWeapon = CurrentWeapon;
		}*/
	}
	
		
}




AWeaponBase* ACharacterBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}





void ACharacterBase::OnFire()
{

}




bool ACharacterBase::Server_SetAiming_Validate(bool WantsToAim)
{
	return true;
}



void ACharacterBase::Server_SetAiming_Implementation(bool WantsToAim)
{
	bIsAiming = WantsToAim;	
	//GetCharacterMovement()->MaxWalkSpeed = WantsToAim ? 400.0f : 600.0f;
	Multi_SetAiming(WantsToAim);
}



bool ACharacterBase::Multi_SetAiming_Validate(bool WantsToAim)
{
	return true;
}


void ACharacterBase::Multi_SetAiming_Implementation(bool WantsToAim)
{
	if (bIsInfected)
	{
		GetCharacterMovement()->MaxWalkSpeed = WantsToAim ? 350.0f : 525.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WantsToAim ? 400.0f : 600.0f;
	}
	
	GetCharacterMovement()->SetJumpAllowed(!WantsToAim);
}




void ACharacterBase::OnAimingStart()
{
	bIsAiming = true;
	OnAimingChanged.Broadcast(true);
	FirstPersonCameraComponent->SetFieldOfView(60.f);
	GetCharacterMovement()->SetJumpAllowed(false);

	if (!HasAuthority())
	{
		Server_SetAiming(true);
	}
	else
	{
		Multi_SetAiming(true);
	}

	GetCharacterMovement()->MaxWalkSpeed = bIsInfected ? 350.0f : 400.0f;


}

void ACharacterBase::OnAimingEnd()
{
	bIsAiming = false;
	OnAimingChanged.Broadcast(false);
	FirstPersonCameraComponent->SetFieldOfView(90.f);
	GetCharacterMovement()->SetJumpAllowed(true);

	if (!HasAuthority())
	{
		Server_SetAiming(false);
	}
	else
	{
		Multi_SetAiming(false);
	}

	GetCharacterMovement()->MaxWalkSpeed = bIsInfected ? 525.0f : 600.0f;

}


void ACharacterBase::EnableChangeWeapon()
{
	bCanChangeWeapon = true;
}


void ACharacterBase::SwitchNextWeapon(bool bIsNext)
{	

	if (CurrentWeapon && bCanChangeWeapon)
	{
		int Difference = bIsNext ? 1 : -1;

		bCanChangeWeapon = false;
		FTimerHandle EnableChanging;
		GetWorldTimerManager().SetTimer(EnableChanging, this, &ACharacterBase::EnableChangeWeapon, bIsRage ? 0.02f : 0.25f, false);


		if (CurrentWeapon->GetCanFire() && !CurrentWeapon->GetIsReloading())
		{
			if (WeaponArray.IsValidIndex(WeaponIndex + Difference))
			{
				if (AWeaponBase* NextWeapon = WeaponArray[WeaponIndex + Difference])
				{
					WeaponIndex += Difference;
					CurrentWeapon->WeaponIsNowInHand(false);
					CurrentWeapon = NextWeapon;
					CurrentWeapon->WeaponIsNowInHand(true);
				}
			}
			else
			{
				WeaponIndex = bIsNext ? 0 : WeaponArray.Num() - 1;
				CurrentWeapon->WeaponIsNowInHand(false);
				CurrentWeapon = WeaponArray[WeaponIndex];
				CurrentWeapon->WeaponIsNowInHand(true);
			}
		}
		
		Server_SwitchWeapon(CurrentWeapon);
	}
}



void ACharacterBase::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}



void ACharacterBase::OnRep_InfectedStateChanged()
{
	if (IsLocallyControlled())
	{
		OnInfectedStateChanged.Broadcast(bIsInfected);
	}

	if (bIsInfected)
	{
		GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? 350.0f : 525.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? 400.0f : 600.0f;
	}
}