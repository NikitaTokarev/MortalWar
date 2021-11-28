// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NaziZombie/Useables/WeaponBase.h"
#include "ZombieBase.generated.h"

class ANaziZombieCharacter;
class USoundWave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);


UCLASS()
class WORLDATWAR_API AZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieBase();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 150.0f;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float Health;	


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditDefaultsOnly)
	float CleanupDelay;

	UPROPERTY(EditDefaultsOnly)
	bool bSimulatePhysicsAfterDeath = true;

	UPROPERTY(ReplicatedUsing = OnRep_Die, BlueprintReadOnly)
	bool bIsDead;

	UFUNCTION()
	void OnRep_Die();

	UFUNCTION(BlueprintCallable)
	void Resurrect();

	UPROPERTY(BlueprintAssignable)
	FOnEnemyDeath OnEnemyDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> DeathSounds;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> HitSounds;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	uint8 GetHitPart(const FString& BoneName);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void DecrementHealth(int16 Damage);

	UFUNCTION(BlueprintNativeEvent, Category = "Death")
	void Die();
	void Die_Implementation();

	int16 GetPointsForHit(uint8 HitPart, FWeaponDamage WeaponDamage, ANaziZombieCharacter* Player);
	int32 GetAmmoForRound() const;


	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_PlaySound(USoundWave* Sound) const;
	bool Multi_PlaySound_Validate(USoundWave* Sound) const;
	void Multi_PlaySound_Implementation(USoundWave* Sound) const;

	USoundWave* GetRandomSound(TArray<USoundWave*> Sounds) const;

	void DeactivateBuff();

public:
	void Hit(ANaziZombieCharacter* Player, FHitResult HitResult);
	void Hit_Knife(ANaziZombieCharacter* Player, float BaseDamage, float RageForKilling);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDamage() const { return Damage; }

	UFUNCTION(BlueprintCallable)
	void ActivateBuff(float ActiveTime, bool bIsEternal = false);
};
