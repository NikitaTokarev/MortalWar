// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "WallWeapon.generated.h"


class UStaticMeshComponent;
class ANaziZombieCharacter;


UCLASS()
class WORLDATWAR_API AWallWeapon : public AInteractableBase
{
	GENERATED_BODY()

public:
	AWallWeapon();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Nazi Zombie Settings")
	USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	UStaticMeshComponent* WallWeaponMesh;
		

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	bool bIsRespawnables = true;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings", meta = (EditCondition = "bIsRespawnables"))
	float RespawnTime;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TArray<AActor*> RespawnPoints;
	
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	bool bChangeLocationOnBegin = true;
	
	virtual void BeginPlay() override;	
	virtual void OnRep_ObjectUsed() override;

	virtual bool CanBeTaken(ANaziZombieCharacter* Player) const;
	virtual void TakeWeapon(ANaziZombieCharacter* Player);

	void SetObjectCanBeUsed();

public:
	virtual void Use(ANaziZombieCharacter* Player) override;
};
