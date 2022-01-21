// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/Weapons/AbstractWeapon.h"
#include "WorldAtWar/Public/WWCoreTypes.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class ANaziZombieCharacter;
class UAnimationAsset;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAmmoChanged, int32, CurrentMagazineAmmo, int32, CurrentTotalAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponHit, class AZombieBase*, TargetEnemy, const FVector, HitLocation);

UENUM(BlueprintType)
enum EHitLocation
{
	None UMETA(DisplayName, "None"),
	Head UMETA(DisplayName, "Head"),
	Torso UMETA(DisplayName, "Torso")
};


//UENUM(BlueprintType)
//enum EWeaponID
//{
//	Colt1911 UMETA(DisplayName, "1911"),
//	M1Carbine UMETA(DisplayName, "M1Carbine"),
//	STG44 UMETA(DisplayName, "STG44")
//};


USTRUCT(BlueprintType)
struct FWeaponDamage
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage = 50.0f;

	float Damage = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageInRage = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeadMultiplier = 3.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TorsoMultiplier = 1.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RageForKilling = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RageForHeadshot = 3.5f;

	float GetDamage(EHitLocation HitLocation)
	{
		switch (HitLocation)
		{
		case Head:
			return Damage * HeadMultiplier;
		case Torso:
			return Damage * TorsoMultiplier;
		default:
			return Damage;
		}
	}
};


UCLASS()
class WORLDATWAR_API AWeaponBase : public AAbstractWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TEnumAsByte<EWeaponID> WeaponID;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimationAsset* FireEmptyAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimationAsset* ReloadAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimationAsset* ReloadEmptyAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimMontage* FPSArmsMontage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	UAnimMontage* ThirdPersonMontage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings", Replicated)
	float DelayBetweenShots;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	FString WeaponName;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
	UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	int32 WeaponMaxAmmo;	

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	int32 MagazineMaxAmmo;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	FWeaponDamage WeaponDamage;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nazi Zombie Settings")
	bool bIsFullAuto = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 CurrentTotalAmmo;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 CurrentMagazineAmmo;	

	UPROPERTY(BlueprintAssignable)
	FAmmoChanged OnAmmoChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponHit OnWeaponHit;
	
	bool bCanFire;
	void ControlFireDelay();

	bool bIsReloading;
	void ControlReloadingDelay();

	int32 BeforeRage_MagazineMaxAmmo;
	int32 BeforeRage_TotalAmmo;
	int32 BeforeRage_MagazineAmmo;
	float BeforeRage_DelayBetweenShots;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TArray<FHitResult> PerformLineTrace(ANaziZombieCharacter* ShootingPlayer);
	TArray<FHitResult> PerformLineTrace(FVector MuzzleLocation, FRotator MuzzleRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Fire(const TArray<FHitResult>& HitResults);
	bool Server_Fire_Validate(const TArray<FHitResult>& HitResults);
	virtual void Server_Fire_Implementation(const TArray<FHitResult>& HitResults);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_Fire(const FHitResult& HitResult);
	bool Multi_Fire_Validate(const FHitResult& HitResult);
	virtual void Multi_Fire_Implementation(const FHitResult& HitResult);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reload(bool bMustUpdateInfo = true);
	bool Server_Reload_Validate(bool bMustUpdateInfo = true);
	virtual void Server_Reload_Implementation(bool bMustUpdateInfo = true);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_Reload(bool bMustUpdateInfo = true);
	bool Multi_Reload_Validate(bool bMustUpdateInfo = true);
	virtual void Multi_Reload_Implementation(bool bMustUpdateInfo = true);

	UFUNCTION(BlueprintCallable)
	void AddBonusDamage(float BonusDamage) {WeaponDamage.Damage += BonusDamage; }

	UFUNCTION(BlueprintCallable)
	void CalculateRicochet(class AZombieBase* TargetEnemy, const FVector HitLocation, const FVector ForwardVector);

public:
	virtual bool Fire(ANaziZombieCharacter* ShootingPlayer);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FWeaponDamage GetWeaponDamage() const { return WeaponDamage; }

	virtual bool Reload(bool bMustUpdateInfo = true);

	// 1 - MagazineAmmo, 2 - TotalAmmo
	TArray<int32> GetCurrentAmmo() const;		

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetMaxMagazineAmmo() const { return MagazineMaxAmmo; }

	UAnimMontage* GetFPSAnimMontage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TEnumAsByte<EWeaponID> GetWeaponID() const { return WeaponID; }

	void WeaponIsNowInHand(bool InHand);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCanFire() const { return bCanFire; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsReloading() const { return bIsReloading; }

	float GetRageForKilling() const { return WeaponDamage.RageForKilling; }
	float GetRageForHeadshot() const { return WeaponDamage.RageForHeadshot; }

	UFUNCTION(BlueprintCallable)
	void RecoveryAmmo(int32 Amount, bool bFull = false);

	void SetCanFire(bool _bCanFire) { bCanFire = _bCanFire; }	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTexture2D* GetWeaponIcon() const { return WeaponIcon; }

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_CallAmmoChangedDelegate(int32 MagazineAmmo, int32 MaxAmmo);
	bool Client_CallAmmoChangedDelegate_Validate(int32 MagazineAmmo, int32 MaxAmmo);
	void Client_CallAmmoChangedDelegate_Implementation(int32 MagazineAmmo, int32 MaxAmmo);

	void ActivateRageMode();
	void DeactivateRageMode();

	bool GetIsFullAuto() const { return bIsFullAuto; }
	
	bool IsFullAmmo() const { return CurrentTotalAmmo == WeaponMaxAmmo && CurrentMagazineAmmo == MagazineMaxAmmo; }

	int8 GetWeaponSlot() const { return int8(WeaponID.GetValue()); }
};
