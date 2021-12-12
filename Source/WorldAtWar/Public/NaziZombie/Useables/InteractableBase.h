// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

class ANaziZombieCharacter;

UCLASS()
class WORLDATWAR_API AInteractableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

protected:

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	FText ObjectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	FText UIMessage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	uint16 Cost;

	UPROPERTY(ReplicatedUsing = OnRep_ObjectUsed)
	bool bIsUsed;

	UFUNCTION()
	virtual void OnRep_ObjectUsed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetUIMessage() const { return UIMessage; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetCost() const { return Cost; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool IsAlreadyUsed(ANaziZombieCharacter* Player) const { return false; }

	virtual void Use(ANaziZombieCharacter* Player);
	virtual bool GetIsUsed() const { return bIsUsed; }	
};
