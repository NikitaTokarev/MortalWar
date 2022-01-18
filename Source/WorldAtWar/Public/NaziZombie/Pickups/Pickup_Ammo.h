// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Pickups/PickupBase.h"
#include "WorldAtWar/Public/WWCoreTypes.h"
#include "Pickup_Ammo.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API APickup_Ammo : public APickupBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	TEnumAsByte<EWeaponID> WeaponID;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	int32 AmmoAmount = 25;

	virtual bool CanTakePickup(class ANaziZombieCharacter* Player) override;
	virtual void GivePickupTo(class ANaziZombieCharacter* Player) override;

public:
	virtual void SetRecoverableValue(float NewValue) override { AmmoAmount = NewValue; };
};
