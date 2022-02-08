// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "AncietBook.generated.h"


class UBoxComponent;
class UStaticMeshComponent;
class ANaziZombieCharacter;

UCLASS()
class WORLDATWAR_API AAncietBook : public AInteractableBase
{
	GENERATED_BODY()

public:
	AAncietBook();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	UBoxComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	UStaticMeshComponent* MeshComp;

	virtual void Use(ANaziZombieCharacter* Player) override;

	virtual void OnRep_ObjectUsed() override;

	/*UFUNCTION(BlueprintNativeEvent)
	void UseObject();
	void UseObject_Implementation();*/
	
};
