// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Pickups/Pickup_Health.h"
#include "Player/NaziZombieCharacter.h"



bool APickup_Health::CanTakePickup(ANaziZombieCharacter* Player)
{
	return !Player->IsFullHealth();
}

void APickup_Health::GivePickupTo(ANaziZombieCharacter* Player)
{
	Player->RecoveryHealth(HealthAmount);	
}
