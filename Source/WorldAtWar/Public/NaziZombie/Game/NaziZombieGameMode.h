// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NaziZombieGameMode.generated.h"


class ANaziZombiePlayerSpawnPoint;
class ANaziZombieZombieSpawnPoint;
class ANaziZombieCharacter;
class AZombieBase;
class ANaziZombieGameState;

UCLASS()
class WORLDATWAR_API ANaziZombieGameMode : public AGameModeBase
{
	GENERATED_BODY()	
	
public:
	ANaziZombieGameMode();

protected:
	ANaziZombieGameState* ZombieGameState;

	bool bHasLoadedSpawnPoints;
	TArray<ANaziZombiePlayerSpawnPoint*> PlayerSpawnPoints;	

	TArray<ANaziZombieZombieSpawnPoint*> ActiveZombieSpawnPoints;
	TArray<ANaziZombieZombieSpawnPoint*> ZombieSpawnPoints;

	UPROPERTY(EditAnywhere, Category = "NaziZombieSettings")
	TSubclassOf<ANaziZombieCharacter> PlayerClass;

	UPROPERTY(EditAnywhere, Category = "NaziZombieSettings")
		TSubclassOf<AZombieBase> ZombieClass;

	FTimerHandle TZombieSpawnHandle;

	uint16 ZombiesRemaining;

protected:
	void CalculateZombieCount();
	void SpawnZombie();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SetSpawnPoints();

public:
	void NewZoneActive(uint8 ZoneNumber);
	void ZombieKilled();
};
