// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/Weapons/WallWeapon.h"
#include "NaziZombie/Useables/WeaponBase.h"
#include "Player/NaziZombieCharacter.h"
#include "Player/NaziZombiePlayerState.h"

#include "Components/StaticMeshComponent.h"



AWallWeapon::AWallWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WallWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(WallWeaponMesh);

	RespawnTime = 5.0f;
}





void AWallWeapon::Use(ANaziZombieCharacter* Player)
{
	if (Player && Player->GetPoints() >= Cost)
	{	
		if (AWeaponBase* ExistingWeapon =  Player->CheckWeaponClass(WeaponClass))
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
		}
	}
}



void AWallWeapon::OnRep_ObjectUsed()
{
	SetActorHiddenInGame(bIsUsed);

	if (HasAuthority() && bIsUsed == true)
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
