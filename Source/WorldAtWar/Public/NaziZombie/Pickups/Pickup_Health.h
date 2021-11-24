// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Pickups/PickupBase.h"
#include "Pickup_Health.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API APickup_Health : public APickupBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	float HealthAmount = 35.0f;

	virtual bool CanTakePickup(class ANaziZombieCharacter* Player) override;
	virtual void GivePickupTo(class ANaziZombieCharacter* Player) override;	
};
