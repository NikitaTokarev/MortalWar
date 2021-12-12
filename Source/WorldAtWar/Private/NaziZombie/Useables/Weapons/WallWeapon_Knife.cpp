// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/Weapons/WallWeapon_Knife.h"
#include "Player/NaziZombieCharacter.h"
#include "NaziZombie/Useables/Weapons/Knife.h"


AWallWeapon_Knife::AWallWeapon_Knife()
{

}

void AWallWeapon_Knife::BeginPlay()
{
	Super::BeginPlay();
}



bool AWallWeapon_Knife::CanBeTaken(ANaziZombieCharacter* Player) const
{
	return !IsAlreadyUsed(Player);
}


void AWallWeapon_Knife::TakeWeapon(ANaziZombieCharacter* Player)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Cast<AActor>(Player);

	if (AKnife* Knife = GetWorld()->SpawnActor<AKnife>(KnifeClass, SpawnParams))
	{
		Player->EquipKnife(Knife);
	}
}


bool AWallWeapon_Knife::IsAlreadyUsed(ANaziZombieCharacter* Player) const
{
	return KnifeClass == Player->GetKnife()->GetClass();
}
