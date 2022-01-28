// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/MysteryBox.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/NaziZombieCharacter.h"
#include "Player/NaziZombiePlayerState.h"
#include "NaziZombie/Useables/Weapons/LootWeaponBase.h"

AMysteryBox::AMysteryBox()
{	
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

	OpenLid = CreateDefaultSubobject<UBoxComponent>(TEXT("OpenLidCollision"));
	OpenLid->SetupAttachment(CollisionComponent);

	MysteryBoxMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	MysteryBoxMesh->SetupAttachment(CollisionComponent);

	Cost = 0;
}



void AMysteryBox::Use(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player && OpenAnimation)
	{
		if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
		{
			if (!PState->DecrementPoints(Cost)) return;

			MysteryBoxMesh->PlayAnimation(OpenAnimation, false);
			bIsUsed = true;
			OnRep_ObjectUsed();			

			Player->ClearInteractable();

			CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);			
			OpenLid->SetRelativeLocation(FVector(0.0f, -50.0f, 60.0f));
			OpenLid->SetBoxExtent(FVector(60.0f, 35.0f, 24.0f));

			if (DroppedWeapons.Num() > 0)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetWorld()->GetFirstPlayerController();
				const FVector LootLocation = MysteryBoxMesh->GetSocketLocation("s_LootPlace");
				const FRotator LootRotation = MysteryBoxMesh->GetSocketRotation("s_LootPlace");
				const FVector LootScale = MysteryBoxMesh->GetRelativeScale3D() + 0.1f;

				const FTransform LootTransform = FTransform(LootRotation, LootLocation, LootScale);				

				LootWeapon = GetWorld()->SpawnActor<ALootWeaponBase>(LootWeaponClass, LootTransform, SpawnParams);

				if (LootWeapon)
				{
					LootWeapon->InitializeLootWeapon(DroppedWeapons[FMath::RandRange(0, DroppedWeapons.Num()-1)]);
					LootWeapon->OnLootPickup.AddUObject(this, &AMysteryBox::TrophiesPickedUp);
				}
			}

			OnObjectUsed.Broadcast();
			SetTimerToDestroy(30.0f);
		}
	}
}


void AMysteryBox::SetTimerToDestroy(float Time)
{
	if (!GetWorld() || bIsClosing) return;

	if (GetWorldTimerManager().IsTimerActive(DestroyTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	}

	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AMysteryBox::DestroyMysteryBox, Time, false);	
}


void AMysteryBox::DestroyMysteryBox()
{
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);	

	if (bIsUsed && !bIsClosing)
	{
		MysteryBoxMesh->PlayAnimation(CloseAnimation, false);
		bIsClosing = true;
	}
	StartDisappearing();

	if (LootWeapon)
	{
		LootWeapon->SetLifeSpan(1.0f);
	}

	SetLifeSpan(2.0f);

}


void AMysteryBox::TrophiesPickedUp()
{
	OnLootTaken.Broadcast();
	
	DestroyMysteryBox();
}
