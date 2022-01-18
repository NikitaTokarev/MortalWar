// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/WeaponBase.h"
#include "Player/NaziZombieCharacter.h"
#include "NaziZombie/Zombie/ZombieBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
//#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SetRootComponent(WeaponMesh);	
	
	WeaponMesh->CastShadow = false;

	bIsFireArms = true;

	WeaponName = "Weapon";	
	WeaponMaxAmmo = 50;
	MagazineMaxAmmo = 10;
	DelayBetweenShots = 0.15f;
	bCanFire = true;
	bIsReloading = false;
	

	CurrentTotalAmmo = WeaponMaxAmmo;
	CurrentMagazineAmmo = MagazineMaxAmmo;	

	BeforeRage_MagazineMaxAmmo = MagazineMaxAmmo;
	BeforeRage_TotalAmmo = CurrentTotalAmmo;
	BeforeRage_MagazineAmmo = CurrentMagazineAmmo;
	BeforeRage_DelayBetweenShots = DelayBetweenShots;
}



// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponMesh->SetHiddenInGame(true);

	WeaponDamage.Damage = WeaponDamage.BaseDamage;
	CurrentTotalAmmo = WeaponMaxAmmo;
	CurrentMagazineAmmo = MagazineMaxAmmo;
		

	WeaponMesh->HideBoneByName(FName("emptyCase_1"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_2"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_3"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_4"), EPhysBodyOp::PBO_None);

	WeaponMesh->HideBoneByName(FName("Magazine2"), EPhysBodyOp::PBO_None);

	if (HasAuthority())
	{
		if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
		{
			if (Player->IsRageMode())
			{
				ActivateRageMode();
			}			
		}
	}

}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentTotalAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentMagazineAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, DelayBetweenShots, COND_OwnerOnly);
}


TArray<FHitResult> AWeaponBase::PerformLineTrace(ANaziZombieCharacter* ShootingPlayer)
{
	FVector Rot = ShootingPlayer->GetFirstPersonCameraComponent()->GetForwardVector();
	FVector Start = ShootingPlayer->GetFirstPersonCameraComponent()->GetComponentLocation() + Rot * 25.0f; 																							
	FVector End = Start + Rot * 5000.0f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(ShootingPlayer);
	FCollisionResponseParams CollisionResponse;

	int a = 99;


	GetWorld()->LineTraceMultiByChannel(OUT HitResults, Start, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams, CollisionResponse);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 8.0f, 0, 5.0f);

	return HitResults;
}

TArray<FHitResult> AWeaponBase::PerformLineTrace(FVector MuzzleLocation, FRotator MuzzleRotation)
{	
	FVector End = MuzzleLocation + MuzzleRotation.Vector() * 5000.0f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if(GetOwner()) CollisionParams.AddIgnoredActor(GetOwner());

	FCollisionResponseParams CollisionResponse;


	GetWorld()->LineTraceMultiByChannel(OUT HitResults, MuzzleLocation, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams, CollisionResponse);
	//DrawDebugLine(GetWorld(), MuzzleLocation, End, FColor::Red, false, 2.0f, 0, 3.0f);

	return HitResults;
}




bool AWeaponBase::Server_Fire_Validate(const TArray<FHitResult>& HitResults)
{
	return true;
}

bool AWeaponBase::Multi_Fire_Validate(const FHitResult& HitResult)
{
	return true;
}




void AWeaponBase::Server_Fire_Implementation(const TArray<FHitResult>& HitResults)
{
	if (CurrentMagazineAmmo > 0)
	{		
		--CurrentMagazineAmmo;

		if (HitResults.Num() > 0)
		{
			for (FHitResult Result : HitResults)
			{
				if (AActor* HitActor = Result.GetActor())
				{
					if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
					{
						if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
						{
							Zombie->Hit(Player, Result);
						}

					}
					//UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *HitActor->GetName());

				}
			}

		}
		if (HitResults.Num() > 0)
		{
			Multi_Fire(HitResults[0]);
		}
		else
		{
			Multi_Fire(FHitResult());
		}
	}

}



void AWeaponBase::Multi_Fire_Implementation(const FHitResult& HitResult)
{
	if (ANaziZombieCharacter* Character = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		if (!Character->IsLocallyControlled() && FireAnimation)
		{
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
			{
				if (ThirdPersonMontage)
				{
					AnimInstance->Montage_Play(ThirdPersonMontage);

					if (Character->GetIsAiming())
					{
						AnimInstance->Montage_JumpToSection(FName("FireADS"), ThirdPersonMontage);
					}
					else
					{
						AnimInstance->Montage_JumpToSection(FName("FireHip"), ThirdPersonMontage);
					}
				}
			}
			WeaponMesh->PlayAnimation(FireAnimation, false);
		}
	}
}


void AWeaponBase::ControlFireDelay()
{
	bCanFire = true;
}

void AWeaponBase::ControlReloadingDelay()
{
	bIsReloading = false;
}


bool AWeaponBase::Fire(ANaziZombieCharacter* ShootingPlayer)
{

	if (CurrentMagazineAmmo > 0 && bCanFire && !bIsReloading)
	{
		bCanFire = false;
		--CurrentMagazineAmmo;		

		if (ShootingPlayer->IsLocallyControlled())
		{
			OnAmmoChanged.Broadcast(CurrentMagazineAmmo, CurrentTotalAmmo);
		}

		if (ShootingPlayer)
		{
			if (UAnimInstance* AnimInstance = ShootingPlayer->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					AnimInstance->Montage_Play(FPSArmsMontage);
					if (ShootingPlayer->GetIsAiming())
					{
						AnimInstance->Montage_JumpToSection(FName("FireADS"), FPSArmsMontage);
					}
					else
					{
						AnimInstance->Montage_JumpToSection(FName("FireHip"), FPSArmsMontage);
					}
				}
			}
		}		


		if (FireAnimation)
		{
			WeaponMesh->PlayAnimation(FireAnimation, false);
		}

		TArray<FHitResult> HitResults = PerformLineTrace(ShootingPlayer);

		if (HitResults.Num() > 0)
		{
			for (FHitResult& Result : HitResults)
			{
				FString HitBone = Result.BoneName.ToString();

				if (AActor* HitActor = Result.GetActor())
				{
					if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
					{
						Zombie->Hit(ShootingPlayer, Result);
						
						if (FMath::RandRange(0.0f, 1.0f) > 0.55f)
						{
							OnWeaponHit.Broadcast(Zombie, Result.Location);
						}						
						break;
					}
					UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *HitActor->GetName());
				}
			}
		}
		if (GetWorld()->IsServer())
		{
			if (HitResults.Num() > 0)
			{
				Multi_Fire(HitResults[0]);
			}
			else
			{
				Multi_Fire(FHitResult());
			}

		}
		else
		{
			Server_Fire(HitResults);
		}

		FTimerHandle FireRateHandle;
		GetWorldTimerManager().SetTimer(FireRateHandle, this, &AWeaponBase::ControlFireDelay, DelayBetweenShots, false);

		return true;
	}
	return false;
}



void AWeaponBase::CalculateRicochet(AZombieBase* TargetEnemy, const FVector HitLocation, const FVector ForwardVector)
{
	FVector End = HitLocation + ForwardVector * 500.0f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(TargetEnemy);

	if (GetOwner()) CollisionParams.AddIgnoredActor(GetOwner());

	FCollisionResponseParams CollisionResponse;


	GetWorld()->LineTraceMultiByChannel(OUT HitResults, HitLocation, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams, CollisionResponse);
	//DrawDebugLine(GetWorld(), HitLocation, End, FColor::Red, false, 2.0f, 0, 3.0f);

	if (HitResults.Num() > 0)
	{
		for (FHitResult& Result : HitResults)
		{
			FString HitBone = Result.BoneName.ToString();

			if (AActor* HitActor = Result.GetActor())
			{
				if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
				{
					Zombie->Hit(Cast<ANaziZombieCharacter>(GetOwner()), Result);
					OnWeaponHit.Broadcast(Zombie, Result.Location);
					break;
				}
				//UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *HitActor->GetName());
			}
		}
	}
}



bool AWeaponBase::Server_Reload_Validate(bool bMustUpdateInfo)
{
	return true;
}



void AWeaponBase::Server_Reload_Implementation(bool bMustUpdateInfo)
{
	Reload();	
}

bool AWeaponBase::Multi_Reload_Validate(bool bMustUpdateInfo)
{
	return true;
}

void AWeaponBase::Multi_Reload_Implementation(bool bMustUpdateInfo)
{
	if (ANaziZombieCharacter* Character = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		if (!Character->IsLocallyControlled() && ReloadAnimation)
		{
			if (UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance())
			{
				if (ThirdPersonMontage)
				{
					AnimInstance->Montage_Play(ThirdPersonMontage);
					AnimInstance->Montage_JumpToSection(FName("Reload"), ThirdPersonMontage);
				}
			}
			WeaponMesh->PlayAnimation(ReloadAnimation, false);
		}
	}
}



bool AWeaponBase::Reload(bool bMustUpdateInfo)
{
	if (CurrentTotalAmmo > 0 && CurrentMagazineAmmo != MagazineMaxAmmo && bCanFire)
	{
		bIsReloading = true;
		
		float ReloadingDelay = 1.0f;

		ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner());

		if (Player)
		{
			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					AnimInstance->Montage_Play(FPSArmsMontage);
					AnimInstance->Montage_JumpToSection(FName("Reload"), FPSArmsMontage);
					ReloadingDelay = FPSArmsMontage->GetSectionLength(2);
				}
			}
		}	

		FTimerHandle ReloadingHandle;
		GetWorldTimerManager().SetTimer(ReloadingHandle, this, &AWeaponBase::ControlReloadingDelay, ReloadingDelay, false);

		if (APawn* Pawn = Cast<APawn>(GetOwner()))
		{
			if (Pawn->IsLocallyControlled() && ReloadAnimation)
			{
				WeaponMesh->PlayAnimation(ReloadAnimation, false);
			}
		}
		

		int Difference = MagazineMaxAmmo - CurrentMagazineAmmo;
		if (CurrentTotalAmmo - Difference >= 0)
		{
			CurrentTotalAmmo -= Difference;
			CurrentMagazineAmmo = MagazineMaxAmmo;
		}
		else
		{
			CurrentMagazineAmmo += CurrentTotalAmmo;
			CurrentTotalAmmo = 0;
		}
	
		if (Player->IsLocallyControlled() && bMustUpdateInfo)
		{
			OnAmmoChanged.Broadcast(CurrentMagazineAmmo, CurrentTotalAmmo);
		}

		if (GetWorld()->IsServer())
		{
			Multi_Reload();
		}
		else
		{
			Server_Reload();
		}


		return true;
	}

	return false;
}

TArray<int32> AWeaponBase::GetCurrentAmmo() const
{
	return {CurrentMagazineAmmo, CurrentTotalAmmo};
}

UAnimMontage* AWeaponBase::GetFPSAnimMontage() const
{
	return FPSArmsMontage;
}

void AWeaponBase::WeaponIsNowInHand(bool InHand)
{
	WeaponMesh->SetHiddenInGame(!InHand);
}

void AWeaponBase::RecoveryAmmo(int32 Amount, bool bFull)
{
	if (bFull)
	{
		CurrentTotalAmmo = WeaponMaxAmmo;
		CurrentMagazineAmmo = MagazineMaxAmmo;
	}
	else
	{
		float Delta_Magazine = FMath::Min(MagazineMaxAmmo - CurrentMagazineAmmo, Amount);
		if (Delta_Magazine > 0)
		{
			CurrentMagazineAmmo += Delta_Magazine;
			Amount -= Delta_Magazine;
		}
		if (Amount > 0 && CurrentTotalAmmo != WeaponMaxAmmo)
		{
			CurrentTotalAmmo = FMath::Min(CurrentTotalAmmo + Amount, WeaponMaxAmmo);
		}
		//CurrentMagazineAmmo = FMath::Min(CurrentMagazineAmmo + Amount, MagazineMaxAmmo);
		//UE_LOG(LogTemp, Error, TEXT("Ammo: %d"), CurrentMagazineAmmo);
	}
	
	Client_CallAmmoChangedDelegate(CurrentMagazineAmmo, CurrentTotalAmmo);
}


bool AWeaponBase::Client_CallAmmoChangedDelegate_Validate(int32 MagazineAmmo, int32 MaxAmmo)
{
	return true;
}


void AWeaponBase::Client_CallAmmoChangedDelegate_Implementation(int32 MagazineAmmo, int32 MaxAmmo)
{
	int a = MagazineAmmo;
	int b = MaxAmmo;

	OnAmmoChanged.Broadcast(MagazineAmmo, MaxAmmo);
}



void AWeaponBase::ActivateRageMode()
{
	if (WeaponDamage.DamageInRage != 1.0f)
	{
		WeaponDamage.Damage += WeaponDamage.BaseDamage * (WeaponDamage.DamageInRage - 1.0f);
	}

	BeforeRage_MagazineMaxAmmo = MagazineMaxAmmo;
	BeforeRage_TotalAmmo = CurrentTotalAmmo;
	BeforeRage_MagazineAmmo = CurrentMagazineAmmo;
	BeforeRage_DelayBetweenShots = DelayBetweenShots;

	MagazineMaxAmmo = CurrentMagazineAmmo = CurrentTotalAmmo = 999;
	DelayBetweenShots /= 4;
}



void AWeaponBase::DeactivateRageMode()
{
	if (WeaponDamage.DamageInRage != 1.0f)
	{
		WeaponDamage.Damage -= WeaponDamage.BaseDamage * (WeaponDamage.DamageInRage - 1.0f);
	}

	MagazineMaxAmmo = BeforeRage_MagazineMaxAmmo;
	CurrentTotalAmmo = BeforeRage_TotalAmmo;
	CurrentMagazineAmmo = BeforeRage_MagazineAmmo;
	DelayBetweenShots = BeforeRage_DelayBetweenShots;
}



