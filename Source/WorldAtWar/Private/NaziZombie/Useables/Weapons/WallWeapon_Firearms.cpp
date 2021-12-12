// Fill out your copyright notice in the Description page of Project Settings.

#include "NaziZombie/Useables/Weapons/WallWeapon_Firearms.h"
#include "NaziZombie/Useables/WeaponBase.h"
#include "Player/NaziZombieCharacter.h"



AWallWeapon_Firearms::AWallWeapon_Firearms()
{

}


void AWallWeapon_Firearms::BeginPlay()
{
	Super::BeginPlay();

}

bool AWallWeapon_Firearms::CanBeTaken(ANaziZombieCharacter* Player) const
{
	AWeaponBase* ExistingWeapon = Player->CheckWeaponClass(WeaponClass);
	if (!ExistingWeapon) return true;

	return !ExistingWeapon->IsFullAmmo();
}



void AWallWeapon_Firearms::TakeWeapon(ANaziZombieCharacter* Player)
{
	if (AWeaponBase* ExistingWeapon = Player->CheckWeaponClass(WeaponClass)) // Already Equip?
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
}



bool AWallWeapon_Firearms::IsAlreadyUsed(ANaziZombieCharacter* Player) const
{
	return Player->CheckWeaponClass(WeaponClass) != nullptr;
}
