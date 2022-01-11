// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "LootWeaponBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLootPickUp);

UCLASS()
class WORLDATWAR_API ALootWeaponBase : public AInteractableBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	class UBoxComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	TSubclassOf<class AAbstractWeapon> WeaponClass;	

	class AAbstractWeapon* AbstractLoot;	

	bool bIsFireArmsWeapon = false;

	virtual void Use(ANaziZombieCharacter* Player) override;

	virtual bool IsAlreadyUsed(ANaziZombieCharacter* Player) const override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetViewModel();

public:	
	FOnLootPickUp OnLootPickup;

	void InitializeLootWeapon(TSubclassOf<AAbstractWeapon> LootWeaponClass);
		
	UFUNCTION(BlueprintCallable)
	bool GetIsFirearms() const { return bIsFireArmsWeapon; }
};
