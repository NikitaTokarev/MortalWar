// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Pickups/PickupBase.h"
#include "Pickup_Rage.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API APickup_Rage : public APickupBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	float RageAmount = 35.0f;

	virtual bool CanTakePickup(class ANaziZombieCharacter* Player) override;
	virtual void GivePickupTo(class ANaziZombieCharacter* Player) override;

public:
	virtual void SetRecoverableValue(float NewValue) override { RageAmount = NewValue; };
};
