// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NaziZombieGameState.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API ANaziZombieGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANaziZombieGameState();

protected:
	uint16 RoundNumber;
	uint8 ZombiesOnMap;
	uint16 TotalZombiesRemaining;
	float ZombieHealth;

public:
	uint16 GetRoundNumber() const;
	void IncrementRoundNumber();

	void SetTotalZombiesRemaining(const uint16& ZombieCount) { TotalZombiesRemaining = ZombieCount; }
	uint16 GetTotalZombiesRemaining() const { return TotalZombiesRemaining; }

	void ZombieKilled();
	float GetZombieHealth() const { return ZombieHealth; }
	
};
