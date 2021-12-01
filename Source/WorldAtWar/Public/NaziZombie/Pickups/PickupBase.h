// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"




UCLASS()
class WORLDATWAR_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere, Category = "Nazi Zombie Settings")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TArray<AActor*> RespawnPoints;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	bool bIsRespawnables = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nazi Zombie Settings", meta = (EditCondition = "bIsRespawnables"))
	float RespawnTime = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	bool bChangeLocationOnBegin = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	class USoundWave* PickupTakenSound;	

	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual bool CanTakePickup(class ANaziZombieCharacter* Player);
	virtual void GivePickupTo(class ANaziZombieCharacter* Player);

public:	
	bool CouldBeTaken() const;

	UFUNCTION(BlueprintCallable)
	void ToggleRespawn(bool bNewFlag) { bIsRespawnables = bNewFlag; }

private:	
	FTimerHandle RespawnTimerHandle;	

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_PickupWasTaken();
	bool Multi_PickupWasTaken_Validate();
	void Multi_PickupWasTaken_Implementation();

	void GetNextRespawnLocation();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_Respawn();
	bool Multi_Respawn_Validate();
	void Multi_Respawn_Implementation();	

	void EnableCollision();

};
