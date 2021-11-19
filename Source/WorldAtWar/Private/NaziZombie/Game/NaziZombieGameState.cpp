// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Game/NaziZombieGameState.h"

ANaziZombieGameState::ANaziZombieGameState()
{
	RoundNumber = 1;
	ZombiesOnMap = 0;
	TotalZombiesRemaining = 5;
	ZombieHealth = 150.0f;
}

uint16 ANaziZombieGameState::GetRoundNumber() const
{
	return RoundNumber;	
}

void ANaziZombieGameState::IncrementRoundNumber()
{
	++RoundNumber;

	if (RoundNumber < 10)
	{
		ZombieHealth += 100;
	}
	else
	{
		ZombieHealth *= 1.1;
	}
}

void ANaziZombieGameState::ZombieKilled()
{
	--TotalZombiesRemaining;
}

