// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "PrayerPlace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailUse);

class UBoxComponent;

UCLASS()
class WORLDATWAR_API APrayerPlace : public AInteractableBase
{
	GENERATED_BODY()

public:
	APrayerPlace();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	UBoxComponent* CollisionComp;

	UPROPERTY(BlueprintAssignable)
	FOnFailUse OnFailUse;

	UFUNCTION(BlueprintNativeEvent)
	bool MightPray(ANaziZombieCharacter* Player);
	bool MightPray_Implementation(ANaziZombieCharacter* Player);

	virtual void OnRep_ObjectUsed() override;

	virtual void Use(ANaziZombieCharacter* Player) override;	

private:
	void AllowPraying();
};
