// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "Barricade.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class ANaziZombieCharacter;
class UAnimationAsset;

UCLASS()
class WORLDATWAR_API ABarricade : public AInteractableBase
{
	GENERATED_BODY()
	
public:
	ABarricade();

	//virtual bool GetIsUsed() const override { return bIsUsed; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	UBoxComponent* CollisionComp;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimationAsset* OpenAnimation;

	/*UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	uint16 Cost;*/

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	uint8 AccessZone;

	/*UPROPERTY(ReplicatedUsing = OnRep_BarricadeUsed)
	bool bIsUsed;*/		
	
	virtual void OnRep_ObjectUsed() override;

protected:
	virtual void BeginPlay() override;
	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const override;
	virtual void Use(ANaziZombieCharacter* Player) override;

public:
	uint8 GetAccessZone() const { return AccessZone; }
};
