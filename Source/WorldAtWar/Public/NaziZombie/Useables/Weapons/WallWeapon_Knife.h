// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/Weapons/WallWeapon.h"
#include "WallWeapon_Knife.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AWallWeapon_Knife : public AWallWeapon
{
	GENERATED_BODY()

public:
	AWallWeapon_Knife();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	TSubclassOf<class AKnife> KnifeClass;

	virtual void BeginPlay() override;

	virtual bool CanBeTaken(ANaziZombieCharacter* Player) const override;
	virtual void TakeWeapon(ANaziZombieCharacter* Player) override;
	
public:
	virtual bool IsAlreadyUsed(ANaziZombieCharacter* Player) const override;
};
