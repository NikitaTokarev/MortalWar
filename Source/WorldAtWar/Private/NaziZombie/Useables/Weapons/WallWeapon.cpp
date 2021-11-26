// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/Weapons/WallWeapon.h"
#include "NaziZombie/Useables/WeaponBase.h"
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

	RespawnTime = 5.0f;


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

	const FString ResultMessage = UIMessage.ToString() + ObjectName.ToString() + "[Cost: " + FString::FromInt(Cost) + "]";

	UIMessage = FText::FromString(ResultMessage);
}



void AWallWeapon::Use(ANaziZombieCharacter* Player)
{
	if (Player && Player->GetPoints() >= Cost)
	{
		if (AWeaponBase* ExistingWeapon = Player->CheckWeaponClass(WeaponClass))
		{
			ExistingWeapon->RecoveryAmmo(0, true);
			if (ExistingWeapon != Player->GetCurrentWeapon())
			{
				Player->SwitchNextWeapon(true);
			}
		}
		else
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Cast<AActor>(Player);

			if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams))
			{
				Player->EquipWeapon(Weapon);
			}
		}


		if (HasAuthority())
		{
			ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>();
			PState->DecrementPoints(Cost);
			bIsUsed = true;
			OnRep_ObjectUsed();

			if (RespawnPoints.Num() != 0)
			{
				int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num() - 1);
				const AActor* RandomPoint = RespawnPoints[RandomIndex];
				checkf(RandomPoint, TEXT("Respawn Point Is Not Valid"));

				SetActorLocationAndRotation(RandomPoint->GetActorLocation(), RandomPoint->GetActorRotation());
			}
		}
	}
}



void AWallWeapon::OnRep_ObjectUsed()
{
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
