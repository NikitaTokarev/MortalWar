// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/CharacterBase.h"
#include "NaziZombieCharacter.generated.h"

class UInputComponent;
class AInteractableBase;
class AWeaponBase;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractChanged, const FString&, OnInteractChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRageChanged, float, NewRage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKnifeAttackDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRageModeFinished);

UCLASS()
class WORLDATWAR_API ANaziZombieCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	ANaziZombieCharacter();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthChanged)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxRage;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float Rage;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsRage;

	FTimerHandle DecrementRageHandle;

	UFUNCTION()
	void OnRep_HealthChanged();

	UPROPERTY(BlueprintAssignable)
	FHealthChanged OnHealthChanged;

	UFUNCTION()
	void GetAnyDamageFromEnemy(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(ReplicatedUsing = OnRep_Death)
	bool bIsDead = false;

	UFUNCTION()
	void OnRep_Death();

	UPROPERTY(BlueprintAssignable)
	FDeathDelegate OnDeath;

	UPROPERTY(BlueprintAssignable)
	FRageModeFinished OnRageFinished;

	UPROPERTY(BlueprintAssignable)
	FRageChanged OnRageChanged;		
	

	UPROPERTY(BlueprintAssignable)
	FInteractChanged OnInteractChanged;

	UPROPERTY(BlueprintAssignable)
	FKnifeAttackDelegate OnKnifing;

	FTimerHandle TInteractTimerHandle;

	UPROPERTY(BlueprintReadOnly)
	AInteractableBase* Interactable;

	UPROPERTY(EditDefaultsOnly, Category = " Nazi Zombie Settings")
	float InteractionRange;

	UPROPERTY(EditDefaultsOnly, Category = " Nazi Zombie Settings")
	TSubclassOf<class AKnife> KnifeClass;

	UPROPERTY(ReplicatedUsing = OnRep_KnifeAttached)
	class AKnife* Knife;

	UFUNCTION()
	void OnRep_KnifeAttached();

protected:
	void Interact();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact(AInteractableBase* InteractingObject);
	bool Server_Interact_Validate(AInteractableBase* InteractingObject);
	void Server_Interact_Implementation(AInteractableBase* InteractingObject);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipWeapon(AWeaponBase* NewWeapon);
	bool Server_EquipWeapon_Validate(AWeaponBase* NewWeapon);
	void Server_EquipWeapon_Implementation(AWeaponBase* NewWeapon);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_EnableRageMode();
	bool Server_EnableRageMode_Validate();
	void Server_EnableRageMode_Implementation();

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_DisableRageMode();
	bool Client_DisableRageMode_Validate();
	void Client_DisableRageMode_Implementation();

	UFUNCTION()
	void DisableRageMode();

	void SetInteractionObject();
	
	virtual void OnFire() override;

	UFUNCTION(BlueprintCallable)
	void OnReload();

	UFUNCTION(BlueprintCallable)
	void OnKnifeAttack();

	void HiddenBody();

	UFUNCTION()
	void DecrementRage();

protected:
	virtual void BeginPlay() override;	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;		
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void EquipWeapon(AWeaponBase* NewWeapon);
	uint32 GetPoints() const;

	UFUNCTION(BlueprintCallable)
	void IncrementRage(float Value);

	AWeaponBase* CheckWeaponClass(TSubclassOf<AWeaponBase> Weapon) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P_BP() const { return Mesh1P; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE class AKnife* GetKnife() const { return Knife; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE	bool GetIsDead() const { return bIsDead; }
	
};
