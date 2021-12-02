// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Game/NaziZombieGameState.h"

ANaziZombieGameState::ANaziZombieGameState()
{
	RoundNumber = 1;
	ZombiesOnMap = 0;
	TotalZombiesRemaining = 0;
	ZombieHealthMult = 1.0f;
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
		ZombieHealthMult *= 1.1f;
	}
	else
	{
		ZombieHealthMult *= 1.05f;
	}
}

void ANaziZombieGameState::SetTotalZombiesRemaining(const uint16& ZombieCount)
{	
	TotalZombiesRemaining = FMath::Min(TotalZombiesRemaining + ZombieCount, 40);
}

void ANaziZombieGameState::ZombieKilled()
{
	--TotalZombiesRemaining;
}



float ANaziZombieGameState::GetZombieHealth(float BaseHealth) const
{
	return FMath::Min(BaseHealth * ZombieHealthMult, BaseHealth * 3.0f);
}

