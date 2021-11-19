// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Game/NaziZombiePlayerSpawnPoint.h"

ANaziZombiePlayerSpawnPoint::ANaziZombiePlayerSpawnPoint()
{
	bIsUsed = false;
}

bool ANaziZombiePlayerSpawnPoint::IsUsed()
{
	return bIsUsed;
}

void ANaziZombiePlayerSpawnPoint::SetUsed(bool Used)
{
	bIsUsed = Used;
}

