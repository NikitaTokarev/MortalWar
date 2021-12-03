// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class AWeaponBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIsAimingChanged, bool, NewAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInfectedState, bool, bNewState);

UCLASS()
class WORLDATWAR_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;
	

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* RageModeCamera;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsRage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TSubclassOf<AWeaponBase> StartingWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TSubclassOf<AWeaponBase> SecondWeaponClass;

	UPROPERTY(ReplicatedUsing = OnRep_AttachWeapon)
	AWeaponBase* CurrentWeapon;	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_InfectedStateChanged)
	bool bIsInfected = false;

	//UPROPERTY(Replicated)
	AWeaponBase* PreviousWeapon;

	UFUNCTION()
	void OnRep_AttachWeapon();

	UFUNCTION()
	void OnRep_InfectedStateChanged();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwitchWeapon(AWeaponBase* NewWeapon);
	bool Server_SwitchWeapon_Validate(AWeaponBase* NewWeapon);
	void Server_SwitchWeapon_Implementation(AWeaponBase* NewWeapon);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_SwitchWeapon(AWeaponBase* NewWeapon);
	bool Client_SwitchWeapon_Validate(AWeaponBase* NewWeapon);
	void Client_SwitchWeapon_Implementation(AWeaponBase* NewWeapon);

	UPROPERTY(BlueprintReadWrite)
	int32 WeaponIndex;

	UPROPERTY(Replicated)
	TArray<AWeaponBase*> WeaponArray;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsAiming;
	
	UPROPERTY(BlueprintAssignable)
	FWeaponChanged OnWeaponChanged;

	UPROPERTY(BlueprintAssignable)
	FIsAimingChanged OnAimingChanged;

	UPROPERTY(BlueprintAssignable)
	FInfectedState OnInfectedStateChanged;

	UFUNCTION(BlueprintCallable)
	virtual void OnAimingStart();

	UFUNCTION(BlueprintCallable)
	virtual void OnAimingEnd();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAiming(bool WantsToAim);
	bool Server_SetAiming_Validate(bool WantsToAim);
	void Server_SetAiming_Implementation(bool WantsToAim);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_SetAiming(bool WantsToAim);
	bool Multi_SetAiming_Validate(bool WantsToAim);
	virtual void Multi_SetAiming_Implementation(bool WantsToAim);

	bool bCanChangeWeapon;
	void EnableChangeWeapon();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
		
		

protected:	
	//void SwitchPreviousWeapon();

	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable)
	virtual void OnFire();	

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	
	// called when turning
	void TurnAtRate(float Rate);

	// called when looking up/down
	void LookUpAtRate(float Rate);
	

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeaponBase* GetCurrentWeapon() const;

public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	void SwitchNextWeapon(bool bIsNext);

	bool GetIsInfected() const { return bIsInfected; }
};
