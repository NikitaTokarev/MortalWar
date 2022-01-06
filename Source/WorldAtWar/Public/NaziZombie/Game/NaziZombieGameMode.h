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


USTRUCT(BlueprintType)
struct FZombieWave
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<class AZombieBase>, float> ZombieWave;	
};


UCLASS()
class WORLDATWAR_API ANaziZombieGameMode : public AGameModeBase
{
	GENERATED_BODY()	
	
public:
	ANaziZombieGameMode();

protected:
	ANaziZombieGameState* ZombieGameState;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class AWeaponBase>> AdditionalWeapons;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaxZombiesOnMap = 30;

	UPROPERTY(EditDefaultsOnly)
	uint8 ZombieSpawnRatio = 10;

	UPROPERTY(EditDefaultsOnly)
	TArray<FZombieWave> ZombiesWaves;	

	bool bHasLoadedSpawnPoints;
	TArray<ANaziZombiePlayerSpawnPoint*> PlayerSpawnPoints;	

	TArray<ANaziZombieZombieSpawnPoint*> ActiveZombieSpawnPoints;
	TArray<ANaziZombieZombieSpawnPoint*> ZombieSpawnPoints;

	UPROPERTY(EditAnywhere, Category = "NaziZombieSettings")
	TSubclassOf<ANaziZombieCharacter> PlayerClass;

	UPROPERTY(EditAnywhere, Category = "NaziZombieSettings")
	TSubclassOf<AZombieBase> ZombieClass;
		

	FTimerHandle TZombieSpawnHandle;
	FTimerHandle TTimesUpHandle;

	uint16 ZombiesRemaining;

protected:
	void CalculateZombieCount();
	void SpawnZombie();

	void TimesUp();

	TSubclassOf<class AZombieBase> GetZombieClass() const;

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SetSpawnPoints();

public:	
	void NewZoneActive(uint8 ZoneNumber);
	void ZombieKilled();

	UFUNCTION(BlueprintCallable)
	void AddNewZombieSpawnPoints(TArray<ANaziZombieZombieSpawnPoint*> NewSpawnPoints);

	TArray<TSubclassOf<class AWeaponBase>> GetAdditionalWeapons() const { return AdditionalWeapons; }
};
