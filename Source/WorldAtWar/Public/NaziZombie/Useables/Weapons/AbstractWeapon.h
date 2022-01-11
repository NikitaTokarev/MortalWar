// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbstractWeapon.generated.h"

UCLASS()
class WORLDATWAR_API AAbstractWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbstractWeapon();

protected:
	bool bIsFireArms;

public:
	bool GetIsFireArms() const { return bIsFireArms; }
};
