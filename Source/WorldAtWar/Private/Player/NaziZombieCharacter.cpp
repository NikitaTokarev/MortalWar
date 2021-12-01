// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NaziZombieCharacter.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "NaziZombie/Zombie/ZombieBase.h"
#include "NaziZombie/Useables/WeaponBase.h"
#include "Player/NaziZombiePlayerState.h"
#include "NaziZombie/Useables/Weapons/Knife.h"
#include "BlueprintFunctionLibrary_Misc.h"

#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

ANaziZombieCharacter::ANaziZombieCharacter()
{
	Interactable = nullptr;
	InteractionRange = 150.0f;

	MaxHealth = 100.0f;
	MaxRage = 100.0f;
	Rage = 0.0f;

	bIsRage = false;
}


void ANaziZombieCharacter::BeginPlay()
{
	Super::BeginPlay();


	if (HasAuthority())
	{
		Health = MaxHealth;
		OnRep_HealthChanged();
		OnTakeAnyDamage.AddDynamic(this, &ANaziZombieCharacter::GetAnyDamageFromEnemy);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		Knife = GetWorld()->SpawnActor<AKnife>(KnifeClass, SpawnParams);
		OnRep_KnifeAttached();

	}


	GetWorld()->GetTimerManager().SetTimer(TInteractTimerHandle, this, &ANaziZombieCharacter::SetInteractionObject, 0.2f, true);
}

void ANaziZombieCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANaziZombieCharacter, Knife);
	DOREPLIFETIME(ANaziZombieCharacter, Health);
	DOREPLIFETIME_CONDITION(ANaziZombieCharacter, Rage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANaziZombieCharacter, bIsRage, COND_OwnerOnly);
	DOREPLIFETIME(ANaziZombieCharacter, bIsDead);	
}


// Called to bind functionality to input
void ANaziZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ANaziZombieCharacter::Interact);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANaziZombieCharacter::OnReload);
	PlayerInputComponent->BindAction("KnifeAttack", IE_Pressed, this, &ANaziZombieCharacter::OnKnifeAttack);

}


void ANaziZombieCharacter::OnRep_HealthChanged()
{
	if (IsLocallyControlled())
	{
		OnHealthChanged.Broadcast(Health);
	}
}







void ANaziZombieCharacter::GetAnyDamageFromEnemy(AActor* DamagedActor, float Damage, const  UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (HasAuthority())
	{
		Health = FMath::Max(Health - Damage, 0.0f);
		OnRep_HealthChanged();

		if (!bIsDead && Health <= 0)
		{
			bIsDead = true;
			OnRep_Death();
		}
	}
}

void ANaziZombieCharacter::OnRep_Death()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	FTimerHandle HiddenBodyHandle;
	GetWorldTimerManager().SetTimer(HiddenBodyHandle, this, &ANaziZombieCharacter::HiddenBody, 5.0f, false);

	OnDeath.Broadcast();

	if (IsLocallyControlled())
	{

	}
	else
	{
		CurrentWeapon->WeaponIsNowInHand(false);
	}
}



void ANaziZombieCharacter::OnRep_KnifeAttached()
{


	if (IsLocallyControlled())
	{
		Knife->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_knifeHolster"));
	}
	else
	{
		Knife->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_knifeHolster"));
	}
}



void ANaziZombieCharacter::EquipWeapon(AWeaponBase* NewWeapon)
{
	if (HasAuthority())
	{
		if (WeaponArray.IsValidIndex(1))
		{
			WeaponArray[1] = NewWeapon;
		}
		else
		{
			WeaponArray.Add(NewWeapon);
			OnEquipNewWeapon.Broadcast(NewWeapon);
		}
		CurrentWeapon = NewWeapon;
		OnRep_AttachWeapon();
	}
	else
	{
		Server_EquipWeapon(NewWeapon);
	}

	WeaponIndex = (WeaponIndex + 1) % WeaponArray.Num();
}




uint32 ANaziZombieCharacter::GetPoints() const
{
	if (auto State = GetPlayerState<ANaziZombiePlayerState>())
	{
		return State->GetPoints();
	}

	return 0;
}




AWeaponBase* ANaziZombieCharacter::CheckWeaponClass(TSubclassOf<AWeaponBase> Weapon) const
{
	for (auto It : WeaponArray)
	{
		if (It->GetClass() == Weapon)
			return It;
	}

	return nullptr;
}




void ANaziZombieCharacter::Interact()
{
	if (Interactable && !Interactable->GetIsUsed() && !bIsDead)
	{
		if (HasAuthority())
		{
			Interactable->Use(this);
		}
		else
		{
			Server_Interact(Interactable);
		}

	}
}

bool ANaziZombieCharacter::Server_Interact_Validate(AInteractableBase* InteractingObject)
{
	return true;
}

void ANaziZombieCharacter::Server_Interact_Implementation(AInteractableBase* InteractingObject)
{
	float Distance = GetDistanceTo(InteractingObject);

	if (Distance < InteractionRange + 80.0f)
	{
		InteractingObject->Use(this);
	}
}




bool ANaziZombieCharacter::Server_EquipWeapon_Validate(AWeaponBase* NewWeapon)
{
	return true;
}

void ANaziZombieCharacter::Server_EquipWeapon_Implementation(AWeaponBase* NewWeapon)
{
	EquipWeapon(NewWeapon);
}



void ANaziZombieCharacter::SetInteractionObject()
{
	FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector Rot = GetFirstPersonCameraComponent()->GetComponentRotation().Vector();
	FVector End = Start + Rot * InteractionRange;

	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionQuery;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByObjectType(OUT HitResult, Start, End, CollisionQuery, CollisionParams);

	AInteractableBase* Temp = Cast<AInteractableBase>(HitResult.GetActor());

	if (Interactable == nullptr && Temp && !Temp->GetIsUsed())
	{
		UE_LOG(LogTemp, Warning, TEXT("IS NOW A VALID POINTER"));
		Interactable = Temp;
		OnInteractChanged.Broadcast(Interactable->GetUIMessage());
	}
	else if (Interactable && (Temp == nullptr || Temp->GetIsUsed()))
	{
		UE_LOG(LogTemp, Warning, TEXT("IS NOW A NULL PTR"));
		Interactable = nullptr;
		OnInteractChanged.Broadcast(FText());
	}

}


void ANaziZombieCharacter::OnFire()
{
	if (CurrentWeapon && !bIsDead)
	{
		if (CurrentWeapon->Fire(this))
		{


		}
	}
}

void ANaziZombieCharacter::OnReload()
{
	if (CurrentWeapon && !bIsDead && !bIsRage)
	{
		CurrentWeapon->Reload();
	}
}


void ANaziZombieCharacter::OnKnifeAttack()
{
	if (Knife && !bIsDead)
	{
		bool bSuccess = Knife->OnKnife(bIsRage);

		if (bSuccess)
		{
			OnKnifing.Broadcast();
		}
	}
}


void ANaziZombieCharacter::HiddenBody()
{
	SetActorHiddenInGame(true);
}





void ANaziZombieCharacter::IncrementRage(float Value)
{
	float PreviousRage = Rage;
	Rage = FMath::Clamp(Rage + Value, 0.0f, MaxRage);

	if (Rage != PreviousRage)
	{
		Client_ChangeRage(Rage);
	}
}



void ANaziZombieCharacter::DecrementRage()
{
	IncrementRage(-5.0f);

	if (Rage - 5 < 0)
	{
		DisableRageMode();
		OnRageFinished.Broadcast();
		GetWorldTimerManager().ClearTimer(DecrementRageHandle);
	}
}




bool ANaziZombieCharacter::Client_ChangeRage_Validate(float NewRage)
{
	return true;
}

void ANaziZombieCharacter::Client_ChangeRage_Implementation(float NewRage)
{
	OnRageChanged.Broadcast(NewRage);
}



void ANaziZombieCharacter::EnableRageMode()
{
	for (auto& It : WeaponArray)
	{
		It->Client_CallAmmoChangedDelegate(999, 999);
	}
	Server_EnableRageMode();
}



bool ANaziZombieCharacter::Server_EnableRageMode_Validate()
{
	return true;
}



void ANaziZombieCharacter::Server_EnableRageMode_Implementation()
{
	bIsRage = true;

	for (auto& It : WeaponArray)
	{
		It->ActivateRageMode();
	}

	Knife->ActivateRageMode();
	if (!GetWorld()) return;

	GetWorldTimerManager().SetTimer(DecrementRageHandle, this, &ANaziZombieCharacter::DecrementRage,
		UBlueprintFunctionLibrary_Misc::IsOnline(this) ? 0.2f : 0.05f, true);
}



void ANaziZombieCharacter::DisableRageMode()
{
	bIsRage = false;

	for (auto& It : WeaponArray)
	{
		It->DeactivateRageMode();
	}
	Knife->DeactivateRageMode();

	if (UBlueprintFunctionLibrary_Misc::IsOnline(this))
	{
		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &ANaziZombieCharacter::Client_DisableRageMode, 0.75f, false);
	}
	else
	{
		Client_DisableRageMode();
	}

}


bool ANaziZombieCharacter::Client_DisableRageMode_Validate()
{
	return true;
}


void ANaziZombieCharacter::Client_DisableRageMode_Implementation()
{
	if (!HasAuthority())
	{
		OnRageFinished.Broadcast();
	}

	for (auto& It : WeaponArray)
	{
		It->Client_CallAmmoChangedDelegate(It->GetCurrentAmmo()[0], It->GetCurrentAmmo()[1]);
	}
}






void ANaziZombieCharacter::RecoveryHealth(float HealthAmount)
{
	if (HasAuthority())
	{
		Health = FMath::Clamp(Health + HealthAmount, 0.1f, MaxHealth);
		OnRep_HealthChanged();

		if (Health <= 0.1f && !bIsDead)
		{
			FTimerHandle TempHandle;
			GetWorldTimerManager().SetTimer(TempHandle, this, &ANaziZombieCharacter::DiseaseFinished, 1.25f, false);
		}
	}
	else
	{
		Server_RecoveryHealth(HealthAmount);
	}
}





bool ANaziZombieCharacter::Server_RecoveryHealth_Validate(float HealthAmount)
{
	return true;
}



void ANaziZombieCharacter::Server_RecoveryHealth_Implementation(float HealthAmount)
{
	RecoveryHealth(HealthAmount);
}




bool ANaziZombieCharacter::Client_OnReload_Validate()
{
	return true;
}



void ANaziZombieCharacter::Client_OnReload_Implementation()
{
	if (CurrentWeapon && !bIsDead)
	{
		CurrentWeapon->Reload(false);
	}
}




bool ANaziZombieCharacter::NeedAmmo(TEnumAsByte<EWeaponID> AmmoType) const
{
	if (bIsRage) return false;

	switch (AmmoType)
	{
	case EWeaponID::Colt1911:
		return !WeaponArray[0]->IsFullAmmo();

	case EWeaponID::M1Carbine:
		if (!WeaponArray.IsValidIndex(1)) return false;
		return !WeaponArray[1]->IsFullAmmo();

	case EWeaponID::STG44:
		return false;

	default:
		return false;
	}
}


void ANaziZombieCharacter::RecoveryAmmo(TEnumAsByte<EWeaponID> AmmoType, int32 AmmoAmount)
{
	switch (AmmoType)
	{
	case EWeaponID::Colt1911:
		if (CurrentWeapon == WeaponArray[0])
		{
			Client_OnReload();
		}


		if (IsLocallyControlled())
		{
			WeaponArray[0]->RecoveryAmmo(AmmoAmount);
		}
		else
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ANaziZombieCharacter::UpdateAmmoAfterPickup_Client,
				WeaponArray[0], AmmoAmount);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
		}

		return;

	case EWeaponID::M1Carbine:
		if (!WeaponArray.IsValidIndex(1)) return;

		if (CurrentWeapon == WeaponArray[1])
		{
			Client_OnReload();
		}


		if (IsLocallyControlled())
		{
			WeaponArray[1]->RecoveryAmmo(AmmoAmount);
		}
		else
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ANaziZombieCharacter::UpdateAmmoAfterPickup_Client,
				WeaponArray[1], AmmoAmount);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
		}

		return;

	case EWeaponID::STG44:
		return;

	default:
		return;
	}
}



void ANaziZombieCharacter::UpdateAmmoAfterPickup_Client(AWeaponBase* Weapon, int32 AmmoAmount)
{
	Weapon->RecoveryAmmo(AmmoAmount);
}



bool ANaziZombieCharacter::Server_ImposeDisease_Validate(float Time, float Damage)
{
	return true;
}



void ANaziZombieCharacter::Server_ImposeDisease_Implementation(float Time, float Damage)
{	
	FTimerDelegate DiseaseTimerDelegate = FTimerDelegate::CreateUObject(this, &ANaziZombieCharacter::RecoveryHealth,
		Damage*(-1));
	GetWorld()->GetTimerManager().SetTimer(DiseaseHandle, DiseaseTimerDelegate, 1.5f, true);

	bIsInfected = true;
	OnRep_InfectedStateChanged();

	FTimerHandle FinishedDiseaseHandle;

	GetWorldTimerManager().SetTimer(FinishedDiseaseHandle, this, &ANaziZombieCharacter::DiseaseFinished, Time + 0.1f, false);
}



void ANaziZombieCharacter::DiseaseFinished()
{
	if (GetWorldTimerManager().IsTimerActive(DiseaseHandle))
	{
		GetWorldTimerManager().ClearTimer(DiseaseHandle);

		bIsInfected = false;
		OnRep_InfectedStateChanged();
	}	
}

