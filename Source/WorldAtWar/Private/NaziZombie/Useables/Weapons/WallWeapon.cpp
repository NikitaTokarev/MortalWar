// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/Weapons/WallWeapon.h"
#include "Player/NaziZombieCharacter.h"
#include "Player/NaziZombiePlayerState.h"
#include "Components/StaticMeshComponent.h"





AWallWeapon::AWallWeapon()
{
	PrimaryActorTick.bCanEverTick = false;	

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Default");
	SetRootComponent(SceneComponent);

	WallWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	WallWeaponMesh->SetupAttachment(GetRootComponent());


	/*WallWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");		
	SetRootComponent(WallWeaponMesh);*/
	


	ObjectName = FText::FromString("M1 Carbine");
	Cost = 500;

	RespawnTime = 35.0f;


}



void AWallWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);

		if (RespawnPoints.Num() != 0 && bChangeLocationOnBegin)
		{
			int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num() - 1);
			const AActor* RandomPoint = RespawnPoints[RandomIndex];
			checkf(RandomPoint, TEXT("Respawn Point Is Not Valid"));

			SetActorLocationAndRotation(RandomPoint->GetActorLocation(), RandomPoint->GetActorRotation());
		}
	}

	/*const FString ResultMessage = UIMessage.ToString() + ObjectName.ToString() + "[Cost: " + FString::FromInt(Cost) + "]";

	UIMessage = FText::FromString(ResultMessage);*/
}



void AWallWeapon::Use(ANaziZombieCharacter* Player)
{
	uint32 FinalCost = CalculateCost(Player);

	if (Player && Player->GetPoints() >= FinalCost && CanBeTaken(Player))
	{		
		TakeWeapon(Player);

		if (HasAuthority())
		{
			ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>();
			PState->DecrementPoints(FinalCost);
			bIsUsed = true;
			OnRep_ObjectUsed();

			if (bIsRespawnables)
			{
				if (RespawnPoints.Num() != 0)
				{
					int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num() - 1);
					const AActor* RandomPoint = RespawnPoints[RandomIndex];
					checkf(RandomPoint, TEXT("Respawn Point Is Not Valid"));

					SetActorLocationAndRotation(RandomPoint->GetActorLocation(), RandomPoint->GetActorRotation());
				}					
			}
			else
			{
				SetLifeSpan(0.2f);
			}
		}
	}
}




bool AWallWeapon::CanBeTaken(ANaziZombieCharacter* Player) const
{
	return true;
}

void AWallWeapon::TakeWeapon(ANaziZombieCharacter* Player)
{
	
}


void AWallWeapon::OnRep_ObjectUsed()
{
	SetActorEnableCollision(!bIsUsed);
	SetActorHiddenInGame(bIsUsed);

	if (HasAuthority() && bIsUsed == true && bIsRespawnables)
	{
		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &AWallWeapon::SetObjectCanBeUsed, RespawnTime, false);
	}
}



void AWallWeapon::SetObjectCanBeUsed()
{
	bIsUsed = false;
	OnRep_ObjectUsed();
}



void AWallWeapon::SetNewCost(int32 NewCost)
{
	Cost = NewCost;

	ChangeUIMessage();
}


void AWallWeapon::MakeRespawnable(float ObjectRespawnTime)
{
	bIsRespawnables = true;
	RespawnTime = ObjectRespawnTime;
}