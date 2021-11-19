// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NaziZombieMainMenuGameMode.generated.h"

class ANaziZombieBeaconHostObject;
class AOnlineBeaconHost;

UCLASS()
class WORLDATWAR_API ANaziZombieMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANaziZombieMainMenuGameMode();

protected:
	UFUNCTION(BlueprintCallable)
	bool CreateHostBeacon();
	
	ANaziZombieBeaconHostObject* HostObject;
	AOnlineBeaconHost* Host;

	UFUNCTION(BlueprintCallable)
	ANaziZombieBeaconHostObject* GetBeaconHost();

};
