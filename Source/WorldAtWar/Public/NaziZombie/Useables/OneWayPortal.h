// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "OneWayPortal.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPortalUsed);

UCLASS()
class WORLDATWAR_API AOneWayPortal : public AInteractableBase
{
	GENERATED_BODY()

public:
	AOneWayPortal();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	FVector DestinationLocation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	FRotator DestinationRotation;

	/*UPROPERTY(BlueprintAssignable)
	FOnPortalUsed OnPortalUsed;*/
	
	virtual void BeginPlay() override;
	virtual void Use(ANaziZombieCharacter* Player);
};
