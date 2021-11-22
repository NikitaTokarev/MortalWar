// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Zombie/ZombieBase.h"
#include "Player/NaziZombieCharacter.h"
#include "Player/NaziZombiePlayerState.h"
#include "NaziZombie/Game/NaziZombieGameMode.h"
#include "NaziZombie/Game/NaziZombieGameState.h"
#include "NaziZombie/Game/NaziZombieGameState.h"

#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
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

}


// Called when the game starts or when spawned
void AZombieBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (ANaziZombieGameState* GS = GetWorld()->GetGameState<ANaziZombieGameState>())
		{
			Health = GS->GetZombieHealth();
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
		Health -= InDamage;
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
	GetMesh()->SetSimulatePhysics(true);
}



void AZombieBase::Die()
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
	}
}


uint8 AZombieBase::GetHitPart(const FString& BoneName)
{

	// limb hit
	if (BoneName.Contains(FString("L")) || BoneName.Contains(FString("R")))
	{
		return 1;
	}
	// torso hit
	else if (BoneName.Contains(FString("Spine")) || BoneName.Contains(FString("Hips")))
	{
		return 2;
	}
	else if (BoneName.Equals(FString("Neck")))
	{
		return 3;
	}
	else if (BoneName.Equals(FString("Head")))
	{
		return 4;
	}

	return 0;
}


int16 AZombieBase::GetPointsForHit(uint8 HitPart, FWeaponDamage WeaponDamage, ANaziZombieCharacter* Player)
{
	TArray<int> DamageScoreKillScore;


	switch (HitPart)
	{
	case 1: // limb
	{
		DamageScoreKillScore.Append({ int(WeaponDamage.GetDamage(EHitLocation::None)), 10, 25 });
		break;
	}
	case 2: // torso
	{
		DamageScoreKillScore.Append({ int(WeaponDamage.GetDamage(EHitLocation::Torso)), 10, 25 });
		break;
	}
	case 3: // neck
	{
		DamageScoreKillScore.Append({ int((WeaponDamage.GetDamage(EHitLocation::Torso)) * 1.2), 20, 50 });
		break;
	}
	case 4: // head
	{
		DamageScoreKillScore.Append({ int(WeaponDamage.GetDamage(EHitLocation::Head)), 50, 100 });
		break;
	}

	default:
	{
		DamageScoreKillScore.Append({ 30, 10, 25 });
		break;
	}
	}


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

		FWeaponDamage WeaponDamage = Player->GetCurrentWeapon()->GetWeaponDamage();

		if (ANaziZombiePlayerState* PState = Cast<ANaziZombiePlayerState>(Player->GetPlayerState()))
		{
			FString BoneName = HitResult.BoneName.ToString();

			if (BoneName.IsEmpty()) return;

			if (uint8 HitPart = GetHitPart(BoneName))
			{
				if (uint8 PointsForHit = GetPointsForHit(HitPart, WeaponDamage, Player))
				{
					PState->IncrementPoints(PointsForHit);
				}
			}
		}
	}
}

void AZombieBase::Hit_Knife(ANaziZombieCharacter* Player, float BaseDamage, float RageForKilling)
{
	if (!Player || bIsDead) return;

	if (ANaziZombiePlayerState* PState = Cast<ANaziZombiePlayerState>(Player->GetPlayerState()))
	{
		{
			PState->IncrementPoints(15);
			if (ANaziZombieGameState* GS = GetWorld()->GetGameState<ANaziZombieGameState>())
			{
				float AdditionalDamage = GS->GetRoundNumber() * 12.5;

				if (Health - (BaseDamage + AdditionalDamage) <= 0)
				{
					PState->AddKill();
					PState->AddKnifing();

					Player->IncrementRage(RageForKilling);
				}

				DecrementHealth(BaseDamage + AdditionalDamage);
			}
			else
				DecrementHealth(BaseDamage);
		}
	}
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
