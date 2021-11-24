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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	float RespawnTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nazi Zombie Settings")
	class USoundWave* PickupTakenSound;

	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual bool CanTakePickup(class ANaziZombieCharacter* Player);
	virtual void GivePickupTo(class ANaziZombieCharacter* Player);

public:
	virtual void Tick(float DeltaTime) override;
	bool CouldBeTaken() const;

private:
	float RotationYaw = 0.0f;
	FTimerHandle RespawnTimerHandle;	

	void PickupWasTaken();

	void GetNextRespawnLocation();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_Respawn(const FVector& Location);
	bool Multi_Respawn_Validate(const FVector& Location);
	void Multi_Respawn_Implementation(const FVector& Location);

	void GenerateRotationYaw();

	void EnableCollision();

};
