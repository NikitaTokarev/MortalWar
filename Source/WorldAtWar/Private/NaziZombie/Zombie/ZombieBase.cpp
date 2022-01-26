// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Zombie/ZombieBase.h"
#include "Player/NaziZombieCharacter.h"
#include "Player/NaziZombiePlayerState.h"
#include "NaziZombie/Game/NaziZombieGameMode.h"
#include "NaziZombie/Game/NaziZombieGameState.h"
#include "NaziZombie/Game/NaziZombieGameState.h"
#include "NaziZombie/Pickups/PickupBase.h"

#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Sets default values
AZombieBase::AZombieBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bIsDead = false;
	CleanupDelay = 5.0f;
	Health = 150.0f;

	/*CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	SetRootComponent(CapsuleCollision);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CapsuleCollision);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));*/
}


// Called when the game starts or when spawned
void AZombieBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (ANaziZombieGameState* GS = GetWorld()->GetGameState<ANaziZombieGameState>())
		{
			MaxHealth = GS->GetZombieHealth(MaxHealth);

			Health = MaxHealth;
		}
		else
		{
			Health = MaxHealth;
		}
	}
}




void AZombieBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZombieBase, bIsDead);
	DOREPLIFETIME(AZombieBase, Health);
}

void AZombieBase::DecrementHealth(int16 InDamage)
{
	if (HasAuthority())
	{
		Health = FMath::Max(Health - InDamage, 0.0f);
		if (Health <= 0)
		{
			Die();
			Multi_PlaySound(GetRandomSound(DeathSounds));
		}
		else
		{
			if (FMath::RandBool())
			{
				Multi_PlaySound(GetRandomSound(HitSounds));
			}
		}

		OnTakeAnyDamage.Broadcast(this, InDamage, nullptr, nullptr, nullptr);
	}
}


void AZombieBase::OnRep_Die()
{
	OnEnemyDeath.Broadcast();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	if (bSimulatePhysicsAfterDeath)
	{
		GetMesh()->SetSimulatePhysics(true);
	}
	
}



void AZombieBase::Resurrect()
{
	Health = MaxHealth * 0.75;
	Damage *= 0.33;
}



void AZombieBase::Die_Implementation()
{
	if (HasAuthority())
	{
		bIsDead = true;
		OnRep_Die();

		if (AController* AIController = GetController())
		{
			AIController->Destroy();
		}
		SetLifeSpan(CleanupDelay);

		if (ANaziZombieGameMode* GM = GetWorld()->GetAuthGameMode<ANaziZombieGameMode>())
		{
			GM->ZombieKilled();
		}

		if (bSpawnPickupAfterDeath && DroppingItems.Num() > 0)
		{
			for (auto& It : DroppingItems)
			{
				if (FMath::RandRange(0.0f, 1.0f) <= It.Value)
				{					

					FTimerHandle TimerHandle;
					FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AZombieBase::SpawnPickupAfterDeath, It.Key,
						GetActorLocation() + FVector(0.0f, 0.0f, HeightOfPickupAboveGround));

					GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
					break;
				}
			}
		}
	}
}



void AZombieBase::SpawnPickupAfterDeath(TSubclassOf<class APickupBase> PickupClass, FVector Location)
{
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = GetWorld()->GetFirstPlayerController();

	GetWorld()->SpawnActor<APickupBase>(PickupClass, Location, FRotator(0.0f), SpawnParam);
}


uint8 AZombieBase::GetHitPart(const FString& BoneName)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Part: %s"), *BoneName);

	// limb hit
	if (BoneName.Contains(FString("Leg")) || BoneName.Contains(FString("Arm")) || BoneName.Contains(FString("Thigh")) || BoneName.Contains(FString("Calf")) || BoneName.Contains(FString("Foot")))
	{
		return 1;
	}
	// torso hit
	else if (BoneName.Contains(FString("Spine")) || BoneName.Contains(FString("Hips")) || BoneName.Contains(FString("Ribcage")))
	{
		return 2;
	}
	else if (BoneName.Equals(FString("Neck")))
	{
		return 3;
	}
	else if (BoneName.Contains(FString("Head")))
	{
		return 4;
	}

	return 0;
}


int16 AZombieBase::GetPointsForHit(uint8 HitPart, FWeaponDamage WeaponDamage, ANaziZombieCharacter* Player, bool bIsAuto)
{
	TArray<int> DamageScoreKillScore;	

	float DamageMultiplier = Player->GetDamageMultiplier();

	switch (HitPart)
	{
	case 1: // limb
	{
		DamageScoreKillScore.Append({ int(WeaponDamage.GetDamage(EHitLocation::None) * DamageMultiplier), bIsAuto? 5 : 10, 25 });
		break;
	}
	case 2: // torso
	{
		DamageScoreKillScore.Append({ int(WeaponDamage.GetDamage(EHitLocation::Torso) * DamageMultiplier), bIsAuto ? 5 : 10, 25 });
		break;
	}
	case 3: // neck
	{
		DamageScoreKillScore.Append({ int((WeaponDamage.GetDamage(EHitLocation::Torso) * DamageMultiplier) * 1.5), bIsAuto? 10 : 20, 50 });
		break;
	}
	case 4: // head
	{
		DamageScoreKillScore.Append({ int(WeaponDamage.GetDamage(EHitLocation::Head) * DamageMultiplier), bIsAuto ? 20 : 50, 100 });
		break;
	}

	default:
	{
		DamageScoreKillScore.Append({ 30, 10, 25 });
		break;
	}
	}

	DamageScoreKillScore[0] *= Resistances.DamageFromFirearms;

	if (bIsAuto)
	{
		DamageScoreKillScore[0] *= Resistances.DamageFromAuto;
	}

	OnTakeDamageRange.Broadcast(Player, DamageScoreKillScore[0]);

	UE_LOG(LogTemp, Error, TEXT("Damage: %d"), DamageScoreKillScore[0]);

	if (Health - DamageScoreKillScore[0] <= 0) // Kill
	{
		DecrementHealth(DamageScoreKillScore[0]);

		/*if (int32 Ammo = GetAmmoForRound())
		{
			Player->GetCurrentWeapon()->RecoveryAmmo(Ammo);
		}*/

		if (ANaziZombiePlayerState* PS = Player->GetPlayerState<ANaziZombiePlayerState>())
		{
			PS->AddKill();

			if (HitPart == 4) // headshot
			{
				PS->AddHeadshot();
				Player->IncrementRage(WeaponDamage.RageForHeadshot);
			}
			else
			{
				Player->IncrementRage(WeaponDamage.RageForKilling);
			}
		}


		return DamageScoreKillScore[2];
	}
	else // Survive
	{
		DecrementHealth(DamageScoreKillScore[0]);
		return DamageScoreKillScore[1];
	}
}

int32 AZombieBase::GetAmmoForRound() const
{
	ANaziZombieGameState* GS = GetWorld()->GetGameState<ANaziZombieGameState>();
	if (GS)
	{
		uint16 Round = FMath::Min(GS->GetRoundNumber(), uint16(2));
		return FMath::RandRange(0, Round);
	}

	return FMath::RandRange(0, 1);
}




void AZombieBase::Hit(ANaziZombieCharacter* Player, FHitResult HitResult)
{
	if (!bIsDead && Player)
	{
		if (HitResult.BoneName.IsNone()) return;
		if (!Player->GetCurrentWeapon()) return;

		AWeaponBase* CurrentWeapon = Player->GetCurrentWeapon();
		FWeaponDamage WeaponDamage = CurrentWeapon->GetWeaponDamage();
		bool bWeaponIsAuto = CurrentWeapon->GetIsFullAuto();

		if (ANaziZombiePlayerState* PState = Cast<ANaziZombiePlayerState>(Player->GetPlayerState()))
		{
			FString BoneName = HitResult.BoneName.ToString();

			if (BoneName.IsEmpty()) return;			

			if (uint8 HitPart = GetHitPart(BoneName))
			{
				if (uint8 PointsForHit = GetPointsForHit(HitPart, WeaponDamage, Player, bWeaponIsAuto))
				{
					if (bShouldTakePoints)
					{
						PState->IncrementPoints(PointsForHit);
					}					
				}
			}
		}
	}
}

void AZombieBase::Hit_Knife(ANaziZombieCharacter* Player, float BaseDamage, float RageForKilling)
{
	if (!Player || bIsDead) return;

	float FinalDamage = BaseDamage * Resistances.DamageFromMelee * Player->GetDamageMultiplier();

	if (ANaziZombiePlayerState* PState = Cast<ANaziZombiePlayerState>(Player->GetPlayerState()))
	{
		if (Health - FinalDamage <= 0)
		{
			PState->AddKill();
			PState->AddKnifing(this);

			if (bShouldTakePoints)
			{
				PState->IncrementPoints(50);
			}
			
			Player->IncrementRage(RageForKilling);
		}
		else if (bShouldTakePoints)
		{
			PState->IncrementPoints(15);
		}

		DecrementHealth(FinalDamage);		
		OnTakeDamageMelee.Broadcast(Player, FinalDamage);
	}
	else
	{
		DecrementHealth(FinalDamage);
	}

	UE_LOG(LogTemp, Error, TEXT("Damage_Knife: %f"), FinalDamage);
}




bool AZombieBase::Multi_PlaySound_Validate(USoundWave* Sound) const
{
	return true;
}

void AZombieBase::Multi_PlaySound_Implementation(USoundWave* Sound) const
{
	if (!GetWorld()) return;
	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld())) return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
}



USoundWave* AZombieBase::GetRandomSound(TArray<USoundWave*> Sounds) const
{
	return Sounds[FMath::RandRange(0, Sounds.Num() - 1)];
}




void AZombieBase::ActivateBuff(float ActiveTime, bool bIsEternal, float HealthPercent)
{
	MaxHealth += MaxHealth * HealthPercent;
	Health = MaxHealth;
	Damage *= 1.33;

	if (!bIsEternal && GetWorld())
	{
		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &AZombieBase::DeactivateBuff, ActiveTime, false);
	}

	PlayBuffVFX();
}




void AZombieBase::DeactivateBuff()
{
	Health /= 1.5;
	MaxHealth /= 1.5;

	Damage /= 1.33;
}


FName AZombieBase::GetChestBone_Implementation() const
{
	return FName("Spine2");
}
