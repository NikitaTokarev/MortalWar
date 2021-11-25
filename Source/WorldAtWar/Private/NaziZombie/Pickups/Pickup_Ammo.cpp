// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Pickups/Pickup_Ammo.h"
#include "Player/NaziZombieCharacter.h"


bool APickup_Ammo::CanTakePickup(ANaziZombieCharacter* Player)
{
    return Player->NeedAmmo(WeaponID);
}


void APickup_Ammo::GivePickupTo(ANaziZombieCharacter* Player)
{
    Player->RecoveryAmmo(WeaponID, AmmoAmount);
}
