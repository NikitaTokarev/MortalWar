// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/Weapons/WallWeapon.h"
#include "WallWeapon_Firearms.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AWallWeapon_Firearms : public AWallWeapon
{
	GENERATED_BODY()

public:
	AWallWeapon_Firearms();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	TSubclassOf<class AWeaponBase> WeaponClass;

protected:
	virtual void BeginPlay() override;
	virtual bool CanBeTaken(ANaziZombieCharacter* Player) const override;
	virtual void TakeWeapon(ANaziZombieCharacter* Player) override;

	virtual uint32 CalculateCost(class ANaziZombieCharacter* Player) const override;
	virtual int GetCost(ANaziZombieCharacter* Player) const override;

public:
	virtual bool IsAlreadyUsed(ANaziZombieCharacter* Player) const override;
	
};
