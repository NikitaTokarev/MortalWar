// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

class ANaziZombieCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectUsed);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	bool bSpecialColor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	FLinearColor SpecialColor = FLinearColor::Black;

	UPROPERTY(ReplicatedUsing = OnRep_ObjectUsed)
	bool bIsUsed;

	UFUNCTION()
	virtual void OnRep_ObjectUsed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnObjectUsed OnObjectUsed;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetUIMessage() const { return UIMessage; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual int GetCost(ANaziZombieCharacter* Player) const { return Cost; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool IsAlreadyUsed(ANaziZombieCharacter* Player) const { return false; }

	virtual void Use(ANaziZombieCharacter* Player);
	virtual bool GetIsUsed() const { return bIsUsed; }	
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasSpecialColor() const { return bSpecialColor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FLinearColor GetSpecialColor() const { return SpecialColor; }
};
