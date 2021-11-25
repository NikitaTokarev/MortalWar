// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Pickups/Pickup_Rage.h"
#include "Player/NaziZombieCharacter.h"



bool APickup_Rage::CanTakePickup(ANaziZombieCharacter* Player)
{
	return !Player->IsFullRage();
}



void APickup_Rage::GivePickupTo(ANaziZombieCharacter* Player)
{
	Player->IncrementRage(RageAmount);
}
