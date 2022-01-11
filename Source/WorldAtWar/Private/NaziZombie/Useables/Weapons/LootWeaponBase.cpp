// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/Weapons/LootWeaponBase.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/NaziZombieCharacter.h"
#include "NaziZombie/Useables/WeaponBase.h"
//#include "NaziZombie/Useables/Weapons/AbstractWeapon.h"
#include "NaziZombie/Useables/Weapons/Knife.h"


// Sets default values
ALootWeaponBase::ALootWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisonComponent"));
	SetRootComponent(CollisionComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(CollisionComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(CollisionComponent);
}

// Called when the game starts or when spawned
void ALootWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(31.2f);
	
}



void ALootWeaponBase::Use(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player)
	{
		if (bIsFireArmsWeapon)
		{
			if (IsAlreadyUsed(Player))
			{
				auto ExistingWeapon = Player->CheckWeaponClass(AbstractLoot->GetClass());
				if (!ExistingWeapon->IsFullAmmo())
				{
					ExistingWeapon->RecoveryAmmo(200, true);
					OnLootPickup.Broadcast();

					Destroy();
				}	
				else
				{
					Player->IncrementPoints(FMath::RandRange(50, 350));
					OnLootPickup.Broadcast();

					AbstractLoot->Destroy();
					Destroy();
				}
			}
			else
			{
				if (AWeaponBase* FireArmsWeapon = Cast<AWeaponBase>(AbstractLoot))
				{
					Player->EquipWeapon(FireArmsWeapon);
					FireArmsWeapon->SetLifeSpan(0.0f);
					OnLootPickup.Broadcast();

					Destroy();
				}				
			}
		}
		else
		{
			if (IsAlreadyUsed(Player))
			{
				Player->IncrementPoints(FMath::RandRange(50, 350));
				OnLootPickup.Broadcast();

				AbstractLoot->Destroy();
				Destroy();
			}
			else
			{
				if (AKnife* MeleeWeapon = Cast<AKnife>(AbstractLoot))
				{
					Player->EquipKnife(MeleeWeapon);
					MeleeWeapon->SetLifeSpan(0.0f);
					OnLootPickup.Broadcast();

					Destroy();
				}				
			}
				
		}
	}
}


bool ALootWeaponBase::IsAlreadyUsed(ANaziZombieCharacter* Player) const
{
	if (bIsFireArmsWeapon)
	{
		return Player->CheckWeaponClass(AbstractLoot->GetClass()) != nullptr;
	}
	else
	{
		return WeaponClass == Player->GetKnife()->GetClass();
	}
}

void ALootWeaponBase::InitializeLootWeapon(TSubclassOf<AAbstractWeapon> LootWeaponClass)
{
	WeaponClass = LootWeaponClass;
	if (HasAuthority() && WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = GetWorld()->GetFirstPlayerController()->GetPawn();

		AbstractLoot = GetWorld()->SpawnActor<AAbstractWeapon>(WeaponClass, FVector(0.0f, 0.0f, 2500.f), FRotator(0.0f), SpawnParams);
		if (AbstractLoot)
		{
			bIsFireArmsWeapon = AbstractLoot->GetIsFireArms();
			AbstractLoot->SetLifeSpan(40.0f);
		}
	}

	SetViewModel();
}

