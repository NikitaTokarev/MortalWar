// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "NaziZombieZombieSpawnPoint.generated.h"


class ABarricade;

UCLASS()
class WORLDATWAR_API ANaziZombieZombieSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	ANaziZombieZombieSpawnPoint();

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	ABarricade* LinkedBarricade;

	uint8 Zone;
	bool bIsActive;

	virtual void BeginPlay() override;

public:
	ABarricade* GetLinkedBarricade() const { return LinkedBarricade; }
	uint8 GetZone() const { return Zone; }

	void SetZone(uint8 ZoneNumber) { Zone = ZoneNumber; }
	bool IsActive() const { return bIsActive; }

	void Activate();
};
