// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "MysteryBox.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLootTaken);


UCLASS()
class WORLDATWAR_API AMysteryBox : public AInteractableBase
{
	GENERATED_BODY()

public:
	AMysteryBox();

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	class UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	class UBoxComponent* OpenLid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	class USkeletalMeshComponent* MysteryBoxMesh;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	class UAnimationAsset* OpenAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	class UAnimationAsset* CloseAnimation;	

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TSubclassOf<class ALootWeaponBase> LootWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TArray<TSubclassOf<class AAbstractWeapon>> DroppedWeapons;

	UPROPERTY(BlueprintAssignable)
	FOnLootTaken OnLootTaken;

	class ALootWeaponBase* LootWeapon = nullptr;

	bool bIsClosing = false;

	FTimerHandle DestroyTimerHandle;

	void DestroyMysteryBox();

	void TrophiesPickedUp();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDisappearing();

public:
	virtual void Use(ANaziZombieCharacter* Player) override;
	
	UFUNCTION(BlueprintCallable)
	void SetTimerToDestroy(float Time);

	UFUNCTION(BlueprintCallable)
	void SetDroppedItems(TArray<TSubclassOf<class AAbstractWeapon>> Drop) { DroppedWeapons = Drop; }
};
