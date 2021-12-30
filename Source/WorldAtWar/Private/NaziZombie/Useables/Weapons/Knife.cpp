// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/Weapons/Knife.h"
#include "Player/NaziZombieCharacter.h"
#include "NaziZombie/Zombie/ZombieBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AKnife::AKnife()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	KnifeMesh = CreateDefaultSubobject<UStaticMeshComponent>("KnifeMesh");
	SetRootComponent(KnifeMesh);
	KnifeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	KnifeMesh->CastShadow = false;

	bCanUse = true;

	BaseDamage = 60.0f;
	RageForKilling = 4.5f;
	DamageMultiplierInRage = 2.45f;

	BeforeRage_BaseDamage = BaseDamage;

	/*CollisionComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	CollisionComponent->SetupAttachment(GetRootComponent());
	CollisionComponent->SetRelativeLocation(FVector(0.0f, 21.0f, 0.0f));
	CollisionComponent->InitBoxExtent(FVector(1.2f, 13.0f, 2.7f));

	FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(this, FName("OnOverlapBegin"));
	CollisionComponent->OnComponentBeginOverlap.Add(ScriptDelegate);*/


}

// Called when the game starts or when spawned
void AKnife::BeginPlay()
{
	Super::BeginPlay();

	//SetReplicates(true);

	//KnifeMesh->SetHiddenInGame(true);

	SetActorHiddenInGame(true);
}





bool AKnife::OnKnife(bool bIsRage)
{
	ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner());
	//KnifeMesh->SetHiddenInGame(false);
	if (!bCanUse || !Player || !Player->GetCurrentWeapon() || Player->GetCurrentWeapon()->GetIsReloading()) return false;
	SetCanAttack(false);

	if (FPSArmsMontage)
	{
		if (Player)
		{
			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(FPSArmsMontage, bIsRage ? 1.25f : 1.0f);
				//FName SectionName = FName(*FString(FString("KnifeAttack") + FString::FromInt(FMath::RandRange(1,2))));
				AnimInstance->Montage_JumpToSection(FName("KnifeAttack1"), FPSArmsMontage);
			}
		}

	}

	if (HasAuthority())
	{
		Multi_KnifeAttack();
	}
	else
		Server_KnifeAttack(bIsRage);

	return true;
}


void AKnife::SetCanAttack(bool bCanAttack)
{
	ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner());
	bCanUse = bCanAttack;
	Player->GetCurrentWeapon()->SetCanFire(bCanAttack);
}




bool AKnife::Server_KnifeAttack_Validate(bool bIsRage)
{
	return true;
}

void AKnife::Server_KnifeAttack_Implementation(bool bIsRage)
{
	OnKnife(bIsRage);
}



bool AKnife::Multi_KnifeAttack_Validate()
{
	return true;
}


void AKnife::Multi_KnifeAttack_Implementation()
{
	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		if (!Player->IsLocallyControlled())
		{
			if (ThirdPersonMontage)
			{
				if (UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance())
				{
					AnimInstance->Montage_Play(ThirdPersonMontage);
					AnimInstance->Montage_JumpToSection(FName("KnifeAttack1"), ThirdPersonMontage);
				}
			}
		}
	}

}

TArray<FHitResult> AKnife::ThrowLineTrace()
{
	FVector Start = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, 35.0f);
	FVector End = Start + (GetOwner()->GetActorForwardVector() + FVector(0.0f, 0.0f, 0.15f)) * 100.f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(GetOwner());
	FCollisionResponseParams CollisionResponse;	
	
	GetWorld()->LineTraceMultiByChannel(OUT HitResults, Start, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams, CollisionResponse);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 3.0f);

	return HitResults;
}




void AKnife::ApplyDamage(TArray<FHitResult>& HitResults)
{
	if (HitResults.Num() > 0)
	{
		for (FHitResult Result : HitResults)
		{
			if (AActor* HitActor = Result.GetActor())
			{
				if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
				{
					if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
					{
						Zombie->Hit_Knife(Player, BaseDamage, RageForKilling);
					}

				}
				//UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *HitActor->GetName());

			}
		}

	}
}

void AKnife::AddDamage(TArray<FHitResult> HitResults)
{
	//TArray<FHitResult> HitResults = ThrowLineTrace();
	ApplyDamage(HitResults);
}



void AKnife::ActivateRageMode()
{
	BaseDamage *= DamageMultiplierInRage;
}


void AKnife::DeactivateRageMode()
{
	BaseDamage = BeforeRage_BaseDamage;
}