// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Game/NaziZombieZombieSpawnPoint.h"
#include "NaziZombie/Useables/Barricade.h"

ANaziZombieZombieSpawnPoint::ANaziZombieZombieSpawnPoint()
{
	Zone = 0;
	bIsActive = false;
}

void ANaziZombieZombieSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

void ANaziZombieZombieSpawnPoint::Activate()
{
	bIsActive = true;
}
