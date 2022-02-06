// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NaziZombieCharacter.h"
#include "NaziZombie/Game/NaziZombieGameMode.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "NaziZombie/Zombie/ZombieBase.h"
#include "NaziZombie/Useables/WeaponBase.h"
#include "Player/NaziZombiePlayerState.h"
#include "NaziZombie/Useables/Weapons/Knife.h"
#include "BlueprintFunctionLibrary_Misc.h"

#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

ANaziZombieCharacter::ANaziZombieCharacter()
{
	InteractableBox = CreateDefaultSubobject<UBoxComponent>("InteractableZone");
	InteractableBox->SetupAttachment(FirstPersonCameraComponent);

	Interactable = nullptr;
	InteractionRange = 150.0f;

	MaxHealth = 100.0f;
	MaxRage = 100.0f;
	Rage = 0.0f;

	bIsRage = false;
}


void ANaziZombieCharacter::BeginPlay()
{
	Super::BeginPlay();	

	if (HasAuthority())
	{
		Health = MaxHealth;
		OnRep_HealthChanged();
		OnTakeAnyDamage.AddDynamic(this, &ANaziZombieCharacter::GetAnyDamageFromEnemy);

		if (!GetWorld()) return;

		
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartingWeaponClass, SpawnParams);

			if (CurrentWeapon)
			{
				PreviousWeapon = CurrentWeapon;
				WeaponArray.Add(CurrentWeapon);
				CurrentWeapon->WeaponIsNowInHand(true);
				OnRep_AttachWeapon();
			}

			WeaponArray.Add(nullptr);
			WeaponArray.Add(nullptr);


			if (ANaziZombieGameMode* GM = GetWorld()->GetAuthGameMode<ANaziZombieGameMode>())
			{
				auto AdditionalWeaponClasses = GM->GetAdditionalWeapons();
				if (AdditionalWeaponClasses.Num() > 0)
				{
					for (auto& WeapClass : AdditionalWeaponClasses)
					{
						AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeapClass, SpawnParams);
						if (Weapon && WeaponArray.IsValidIndex(Weapon->GetWeaponSlot()))
						{
							WeaponArray[Weapon->GetWeaponSlot()] = Weapon;
							if (Weapon->GetWeaponSlot() == 0)
							{
								CurrentWeapon->WeaponIsNowInHand(false);
								CurrentWeapon->SetLifeSpan(0.1f);

								CurrentWeapon = Weapon;
								PreviousWeapon = CurrentWeapon;
								OnRep_AttachWeapon();
							}
						}
					}

					TSubclassOf<AKnife> WeaponKnifeClass = GM->GetKnifeClass();
					if (WeaponKnifeClass == nullptr)
					{
						WeaponKnifeClass = KnifeClass;
					}

					Knife = GetWorld()->SpawnActor<AKnife>(WeaponKnifeClass, SpawnParams);
					OnRep_KnifeAttached();
				}

			}
			else
			{
				Knife = GetWorld()->SpawnActor<AKnife>(KnifeClass, SpawnParams);
				OnRep_KnifeAttached();
			}


			/*if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(SecondWeaponClass, SpawnParams))
			{
				Weapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_weaponSocket"));
				WeaponArray.Add(Weapon);
			}*/
		}


	//GetWorld()->GetTimerManager().SetTimer(TInteractTimerHandle, this, &ANaziZombieCharacter::SetInteractionObject, 0.3f, true);
	InteractableBox->OnComponentBeginOverlap.AddDynamic(this, &ANaziZombieCharacter::OnInteractableBoxStartOverlap);
	InteractableBox->OnComponentEndOverlap.AddDynamic(this, &ANaziZombieCharacter::OnInteractableBoxEndOverlap);
}

void ANaziZombieCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANaziZombieCharacter, Knife);
	DOREPLIFETIME(ANaziZombieCharacter, Health);
	DOREPLIFETIME_CONDITION(ANaziZombieCharacter, Rage, COND_OwnerOnly);	
	DOREPLIFETIME(ANaziZombieCharacter, bIsDead);	
}


// Called to bind functionality to input
void ANaziZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ANaziZombieCharacter::Interact);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANaziZombieCharacter::OnReload);
	PlayerInputComponent->BindAction("KnifeAttack", IE_Pressed, this, &ANaziZombieCharacter::OnKnifeAttack);

}



void ANaziZombieCharacter::OnInteractableBoxStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetInteractionObject(OtherActor);
}



void ANaziZombieCharacter::OnInteractableBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == Interactable)
	{
		Interactable = nullptr;
		OnInteractChanged.Broadcast(nullptr);
	}
}



void ANaziZombieCharacter::SetInteractionObject(AActor* OtherActor)
{
	/*FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation() + GetActorForwardVector() * 10.0f;
	FVector Rot = GetFirstPersonCameraComponent()->GetComponentRotation().Vector();
	FVector End = Start + Rot * InteractionRange;

	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionQuery;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByObjectType(OUT HitResult, Start, End, CollisionQuery, CollisionParams);*/

	AInteractableBase* Temp = Cast<AInteractableBase>(OtherActor);

	if (Interactable == nullptr && Temp && !Temp->GetIsUsed())
	{
		//UE_LOG(LogTemp, Warning, TEXT("IS NOW A VALID POINTER"));
		Interactable = Temp;
		OnInteractChanged.Broadcast(Interactable);
	}
	else if (Interactable && (Temp == nullptr || Temp->GetIsUsed()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("IS NOW A NULL PTR"));
		Interactable = nullptr;
		OnInteractChanged.Broadcast(nullptr);
	}
	
}




void ANaziZombieCharacter::OnRep_HealthChanged()
{
	if (IsLocallyControlled())
	{
		OnHealthChanged.Broadcast(Health);
	}
}







void ANaziZombieCharacter::GetAnyDamageFromEnemy(AActor* DamagedActor, float Damage, const  UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (HasAuthority())
	{	
		float FinalDamage = Damage / Armor;

		Health = FMath::Max(Health - FinalDamage, 0.0f);
		OnRep_HealthChanged();

		if (!bIsDead && Health <= 0)
		{
			bIsDead = true;
			OnRep_Death();
		}
	}
}

void ANaziZombieCharacter::OnRep_Death()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	FTimerHandle HiddenBodyHandle;
	GetWorldTimerManager().SetTimer(HiddenBodyHandle, this, &ANaziZombieCharacter::HiddenBody, 5.0f, false);

	OnDeath.Broadcast();

	if (IsLocallyControlled())
	{

	}
	else
	{
		CurrentWeapon->WeaponIsNowInHand(false);
	}
}



void ANaziZombieCharacter::OnRep_KnifeAttached()
{

	if (IsLocallyControlled())
	{
		Knife->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_knifeHolster"));
	}
	else
	{
		Knife->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_knifeHolster"));
	}
}



void ANaziZombieCharacter::EquipWeapon(AWeaponBase* NewWeapon)
{
	//UE_LOG(LogTemp, Warning, TEXT("%d"), NewWeapon->GetWeaponSlot());

	if (HasAuthority())
	{
		int8 WeaponSlot = NewWeapon->GetWeaponSlot();

		if (WeaponArray.IsValidIndex(WeaponSlot))
		{
			if (WeaponArray[WeaponSlot] != nullptr)
			{
				WeaponArray[WeaponSlot]->SetLifeSpan(0.1f);
			}				
			
			WeaponArray[WeaponSlot] = NewWeapon;			
		}
		/*else
		{
			WeaponArray.Add(NewWeapon);
			OnEquipNewWeapon.Broadcast(NewWeapon);
		}*/

		if (!CurrentWeapon->GetIsReloading())
		{
			CurrentWeapon = NewWeapon;
			OnRep_AttachWeapon();
			WeaponIndex = WeaponSlot;
		}		
	}
	else
	{
		Server_EquipWeapon(NewWeapon);
	}	

	OnEquipNewWeapon.Broadcast(NewWeapon);
}



void ANaziZombieCharacter::EquipKnife(AKnife* NewKnife)
{
	Knife = NewKnife;

	OnEquipNewWeapon.Broadcast(nullptr);
}




uint32 ANaziZombieCharacter::GetPoints() const
{
	if (auto State = GetPlayerState<ANaziZombiePlayerState>())
	{
		return State->GetPoints();
	}

	return 0;
}




AWeaponBase* ANaziZombieCharacter::CheckWeaponClass(TSubclassOf<AWeaponBase> Weapon) const
{
	if (!this)
		return nullptr;

	for (auto It : WeaponArray)
	{
		if (It && It->GetClass() == Weapon)
			return It;
	}

	return nullptr;
}




void ANaziZombieCharacter::Interact()
{
	if (Interactable && !Interactable->GetIsUsed() && !bIsDead)
	{
		OnInteracted.Broadcast(Interactable);

		if (HasAuthority())
		{
			Interactable->Use(this);
		}
		else
		{
			Server_Interact(Interactable);
		}		
	}
}

bool ANaziZombieCharacter::Server_Interact_Validate(AInteractableBase* InteractingObject)
{
	return true;
}

void ANaziZombieCharacter::Server_Interact_Implementation(AInteractableBase* InteractingObject)
{
	float Distance = GetDistanceTo(InteractingObject);

	if (Distance < InteractionRange + 80.0f)
	{
		InteractingObject->Use(this);
	}
	
}




bool ANaziZombieCharacter::Server_EquipWeapon_Validate(AWeaponBase* NewWeapon)
{
	return true;
}

void ANaziZombieCharacter::Server_EquipWeapon_Implementation(AWeaponBase* NewWeapon)
{
	EquipWeapon(NewWeapon);
}





void ANaziZombieCharacter::OnFire()
{
	if (CurrentWeapon && !bIsDead)
	{
		if (CurrentWeapon->Fire(this))
		{


		}
	}
}

void ANaziZombieCharacter::OnReload()
{
	if (CurrentWeapon && !bIsDead && !bIsRage)
	{
		CurrentWeapon->Reload();
	}
}


void ANaziZombieCharacter::OnKnifeAttack()
{
	if (Knife && !bIsDead)
	{
		bool bSuccess = Knife->OnKnife(bIsRage);

		if (bSuccess)
		{
			OnKnifing.Broadcast();
		}
	}
}


void ANaziZombieCharacter::HiddenBody()
{
	SetActorHiddenInGame(true);
}





void ANaziZombieCharacter::IncrementRage(float Value)
{
	float PreviousRage = Rage;
	Rage = FMath::Clamp(Rage + Value, 0.0f, MaxRage);

	if (Rage != PreviousRage)
	{
		Client_ChangeRage(Rage);
	}
}



void ANaziZombieCharacter::DecrementRage()
{
	IncrementRage(-5.0f);

	if (Rage - 5 < 0)
	{
		DisableRageMode();
		OnRageFinished.Broadcast();
		GetWorldTimerManager().ClearTimer(DecrementRageHandle);
	}
}




bool ANaziZombieCharacter::Client_ChangeRage_Validate(float NewRage)
{
	return true;
}

void ANaziZombieCharacter::Client_ChangeRage_Implementation(float NewRage)
{
	OnRageChanged.Broadcast(NewRage);
}



void ANaziZombieCharacter::EnableRageMode()
{
	for (auto& Weapon : WeaponArray)
	{
		if (Weapon)
		{
			Weapon->Client_CallAmmoChangedDelegate(999, 999);
		}		
	}
	Server_EnableRageMode();
}



bool ANaziZombieCharacter::Server_EnableRageMode_Validate()
{
	return true;
}



void ANaziZombieCharacter::Server_EnableRageMode_Implementation()
{
	bIsRage = true;

	for (auto& Weapon : WeaponArray)
	{
		if (Weapon)
		{
			Weapon->ActivateRageMode();
		}			
	}

	Knife->ActivateRageMode();
	if (!GetWorld()) return;

	GetWorldTimerManager().SetTimer(DecrementRageHandle, this, &ANaziZombieCharacter::DecrementRage,
		UBlueprintFunctionLibrary_Misc::IsOnline(this) ? 0.2f : 0.05f, true);
}



void ANaziZombieCharacter::DisableRageMode()
{
	bIsRage = false;

	for (auto& Weapon : WeaponArray)
	{
		if (Weapon)
		{
			Weapon->DeactivateRageMode();
		}		
	}
	Knife->DeactivateRageMode();

	if (UBlueprintFunctionLibrary_Misc::IsOnline(this))
	{
		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &ANaziZombieCharacter::Client_DisableRageMode, 0.75f, false);
	}
	else
	{
		Client_DisableRageMode();
	}

}


bool ANaziZombieCharacter::Client_DisableRageMode_Validate()
{
	return true;
}


void ANaziZombieCharacter::Client_DisableRageMode_Implementation()
{
	if (!HasAuthority())
	{
		OnRageFinished.Broadcast();
	}

	for (auto& Weapon : WeaponArray)
	{
		if (Weapon)
		{
			Weapon->Client_CallAmmoChangedDelegate(Weapon->GetCurrentAmmo()[0], Weapon->GetCurrentAmmo()[1]);
		}		
	}
}






void ANaziZombieCharacter::RecoveryHealth(float HealthAmount)
{
	if (HasAuthority())
	{
		Health = FMath::Clamp(Health + HealthAmount, 0.1f, MaxHealth);
		OnRep_HealthChanged();

		if (Health <= 0.1f && !bIsDead)
		{
			FTimerHandle TempHandle;
			GetWorldTimerManager().SetTimer(TempHandle, this, &ANaziZombieCharacter::DiseaseFinished, 1.25f, false);
		}
	}
	else
	{
		Server_RecoveryHealth(HealthAmount);
	}
}





bool ANaziZombieCharacter::Server_RecoveryHealth_Validate(float HealthAmount)
{
	return true;
}



void ANaziZombieCharacter::Server_RecoveryHealth_Implementation(float HealthAmount)
{
	RecoveryHealth(HealthAmount);
}




bool ANaziZombieCharacter::Client_OnReload_Validate()
{
	return true;
}



void ANaziZombieCharacter::Client_OnReload_Implementation()
{
	if (CurrentWeapon && !bIsDead)
	{
		CurrentWeapon->Reload(false);
	}
}




bool ANaziZombieCharacter::NeedAmmo(TEnumAsByte<EWeaponID> AmmoType) const
{
	if (bIsRage) return false;
		
	switch (AmmoType)
	{
	case EWeaponID::Colt1911:
		return WeaponArray[0] && !WeaponArray[0]->IsFullAmmo();

	case EWeaponID::M1Carbine:		
		return WeaponArray[1] && !WeaponArray[1]->IsFullAmmo();

	case EWeaponID::STG44:
		return WeaponArray[2] && !WeaponArray[2]->IsFullAmmo();

	default:
		return false;
	}
}


void ANaziZombieCharacter::RecoveryAmmo(TEnumAsByte<EWeaponID> AmmoType, int32 AmmoAmount)
{
	switch (AmmoType)
	{
	case EWeaponID::Colt1911:
		if (CurrentWeapon == WeaponArray[0])
		{
			Client_OnReload();
		}


		if (IsLocallyControlled())
		{
			WeaponArray[0]->RecoveryAmmo(AmmoAmount);
		}
		else
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ANaziZombieCharacter::UpdateAmmoAfterPickup_Client,
				WeaponArray[0], AmmoAmount);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
		}

		return;

	case EWeaponID::M1Carbine:
		if (!WeaponArray.IsValidIndex(1)) return;

		if (CurrentWeapon == WeaponArray[1])
		{
			Client_OnReload();
		}


		if (IsLocallyControlled())
		{
			WeaponArray[1]->RecoveryAmmo(AmmoAmount);
		}
		else
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ANaziZombieCharacter::UpdateAmmoAfterPickup_Client,
				WeaponArray[1], AmmoAmount);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
		}

		return;

	case EWeaponID::STG44:

		if (CurrentWeapon == WeaponArray[2])
		{
			Client_OnReload();
		}

		if (IsLocallyControlled())
		{
			WeaponArray[2]->RecoveryAmmo(AmmoAmount);
		}
		else
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ANaziZombieCharacter::UpdateAmmoAfterPickup_Client,
				WeaponArray[2], AmmoAmount);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.5f, false);
		}

		return;
	default:
		return;
	}
}



void ANaziZombieCharacter::UpdateAmmoAfterPickup_Client(AWeaponBase* Weapon, int32 AmmoAmount)
{
	Weapon->RecoveryAmmo(AmmoAmount);
}



bool ANaziZombieCharacter::Server_ImposeDisease_Validate(float Time, float Damage)
{
	return true;
}



void ANaziZombieCharacter::Server_ImposeDisease_Implementation(float Time, float Damage)
{	
	FTimerDelegate DiseaseTimerDelegate = FTimerDelegate::CreateUObject(this, &ANaziZombieCharacter::RecoveryHealth,
		Damage*(-1));
	GetWorld()->GetTimerManager().SetTimer(DiseaseHandle, DiseaseTimerDelegate, 1.5f, true);

	bIsInfected = true;
	OnRep_InfectedStateChanged();

	FTimerHandle FinishedDiseaseHandle;

	GetWorldTimerManager().SetTimer(FinishedDiseaseHandle, this, &ANaziZombieCharacter::DiseaseFinished, Time + 0.1f, false);
}



void ANaziZombieCharacter::DiseaseFinished()
{
	if (GetWorldTimerManager().IsTimerActive(DiseaseHandle))
	{
		GetWorldTimerManager().ClearTimer(DiseaseHandle);

		bIsInfected = false;
		OnRep_InfectedStateChanged();
	}	
}



void ANaziZombieCharacter::ClearInteractable()
{
	if (HasAuthority() && Interactable)
	{
		SetInteractionObject(nullptr);
	}
}


UClass* ANaziZombieCharacter::GetInteractableCompClass() const
{
	return InteractableBox->GetClass();
}


void ANaziZombieCharacter::IncrementPoints(int32 Value)
{
	if (HasAuthority())
	{
		if (auto State = GetPlayerState<ANaziZombiePlayerState>())
		{
			State->IncrementPoints(Value);
		}
	}
}


void ANaziZombieCharacter::AddBonusMaxHealth(float HealthAmount, bool bRecoveryHealth)
{
	MaxHealth += HealthAmount;
	
	if (bRecoveryHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health = FMath::Min(Health, MaxHealth);
	}

	OnHealthChanged.Broadcast(Health);
}

