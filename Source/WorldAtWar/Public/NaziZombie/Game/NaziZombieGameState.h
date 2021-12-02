// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NaziZombieGameState.generated.h"




UCLASS()
class WORLDATWAR_API ANaziZombieGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANaziZombieGameState();

protected:
	/*UPROPERTY(EditDefaultsOnly)
	TArray<TMap<TSubclassOf<class AZombieBase>, float>> ZombiesWaves;*/	

	uint16 RoundNumber;
	uint8 ZombiesOnMap;
	uint16 TotalZombiesRemaining;
	float ZombieHealthMult;

public:
	uint16 GetRoundNumber() const;
	void IncrementRoundNumber();

	void SetTotalZombiesRemaining(const uint16& ZombieCount);
	uint16 GetTotalZombiesRemaining() const { return TotalZombiesRemaining; }

	void ZombieKilled();
	float GetZombieHealth(float BaseHealth) const;
	
};
