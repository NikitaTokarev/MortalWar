// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/CharacterBase.h"
#include "WorldAtWar/Public/WWCoreTypes.h"
#include "NaziZombieCharacter.generated.h"

class UInputComponent;
class AInteractableBase;
class AWeaponBase;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractChanged, AInteractableBase*, InteractableObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteracted, AInteractableBase*, InteractableObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRageChanged, float, NewRage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKnifeAttackDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRageModeFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipNewWeapon, class AWeaponBase*, NewWeapon);

UCLASS()
class WORLDATWAR_API ANaziZombieCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	ANaziZombieCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* InteractableBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthChanged)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxRage;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float Rage;

	UPROPERTY(BlueprintReadOnly)
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Armor = 1.0f;
	
	FTimerHandle DecrementRageHandle;

	FTimerHandle DiseaseHandle;

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
	FOnInteracted OnInteracted;

	UPROPERTY(BlueprintAssignable)
	FKnifeAttackDelegate OnKnifing;

	UPROPERTY(BlueprintAssignable)
	FOnEquipNewWeapon OnEquipNewWeapon;

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
	void Server_RecoveryHealth(float HealthAmount);
	bool Server_RecoveryHealth_Validate(float HealthAmount);
	void Server_RecoveryHealth_Implementation(float HealthAmount);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_OnReload();
	bool Client_OnReload_Validate();
	void Client_OnReload_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipWeapon(AWeaponBase* NewWeapon);
	bool Server_EquipWeapon_Validate(AWeaponBase* NewWeapon);
	void Server_EquipWeapon_Implementation(AWeaponBase* NewWeapon);

	UFUNCTION(BlueprintCallable)
	void EnableRageMode();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EnableRageMode();
	bool Server_EnableRageMode_Validate();
	void Server_EnableRageMode_Implementation();

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_DisableRageMode();
	bool Client_DisableRageMode_Validate();
	void Client_DisableRageMode_Implementation();

	UFUNCTION()
	void DisableRageMode();

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_ChangeRage(float NewRage);
	bool Client_ChangeRage_Validate(float NewRage);
	void Client_ChangeRage_Implementation(float NewRage);



	void SetInteractionObject(AActor* OtherActor);

	UFUNCTION()
	void OnInteractableBoxStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
		OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractableBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
		OtherComp, int32 OtherBodyIndex);
	
	virtual void OnFire() override;

	UFUNCTION(BlueprintCallable)
	void OnReload();

	UFUNCTION(BlueprintCallable)
	void OnKnifeAttack();

	void HiddenBody();

	UFUNCTION()
	void DecrementRage();

	UFUNCTION()
	void UpdateAmmoAfterPickup_Client(class AWeaponBase* Weapon, int32 AmmoAmount);

protected:
	virtual void BeginPlay() override;	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;		
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AWeaponBase* NewWeapon);

	UFUNCTION(BlueprintCallable)
	void EquipKnife(AKnife* NewKnife);

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
	
	bool IsFullHealth() const { return Health == MaxHealth; }
	bool IsFullRage() const { return Rage == MaxRage; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool NeedAmmo(TEnumAsByte<EWeaponID> AmmoType) const;

	UFUNCTION(BlueprintCallable)
	void RecoveryHealth(float HealthAmount);
	void RecoveryAmmo(TEnumAsByte<EWeaponID> AmmoType, int32 AmmoAmount);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_ImposeDisease(float Time, float Damage);
	bool Server_ImposeDisease_Validate(float Time, float Damage);
	void Server_ImposeDisease_Implementation(float Time, float Damage);

	UFUNCTION(BlueprintCallable)
	void DiseaseFinished();

	void ClearInteractable();
	UClass* GetInteractableCompClass() const;

	UFUNCTION(BlueprintCallable)
	void IncrementPoints(int32 Value);

	UFUNCTION(BlueprintCallable)
	void AddBonusMaxHealth(float HealthAmount, bool bRecoveryHealth = false);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDamageMultiplier() const { return DamageMultiplier; }

	UFUNCTION(BlueprintCallable)
	void ChangeDamageMultiplier(float DamageValue) { DamageMultiplier += DamageValue; }

	UFUNCTION(BlueprintCallable)
	void ChangePlayerArmor(float ArmorValue) { Armor += ArmorValue; }
};
