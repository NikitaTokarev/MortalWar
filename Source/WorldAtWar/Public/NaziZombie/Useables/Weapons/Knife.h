// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/Weapons/AbstractWeapon.h"
#include "Knife.generated.h"



UCLASS()
class WORLDATWAR_API AKnife : public AAbstractWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnife();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	class UStaticMeshComponent* KnifeMesh;
	

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimMontage* FPSArmsMontage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimMontage* ThirdPersonMontage;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_KnifeAttack(bool bIsRage);
	bool Server_KnifeAttack_Validate(bool bIsRage);
	void Server_KnifeAttack_Implementation(bool bIsRage);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_KnifeAttack();
	bool Multi_KnifeAttack_Validate();
	void Multi_KnifeAttack_Implementation();

	TArray<FHitResult> ThrowLineTrace();
	void ApplyDamage(TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable)
	void AddDamage(TArray<FHitResult> HitResults);

	class ANaziZombieGameState* GameState;

	bool bCanUse;	

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	USoundWave* AttackSound;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	float RageForKilling;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	float DamageMultiplierInRage;

	float BeforeRage_BaseDamage;

public:
	bool OnKnife(bool bIsRage);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE class UStaticMeshComponent* GetKnifeMesh() const { return KnifeMesh; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE USoundWave* GetAttackSound() const { return AttackSound; }

	UFUNCTION(BlueprintCallable)
	void SetCanAttack(bool bCanAttack);

	void ActivateRageMode();
	void DeactivateRageMode();

	float GetBaseDamage() const { return BaseDamage; }
	float GetRageForKilling() const { return RageForKilling; }
	

};
