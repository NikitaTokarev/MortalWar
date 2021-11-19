// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/Barricade.h"
#include "NaziZombie/Game/NaziZombieGameMode.h"
#include "Player/NaziZombieCharacter.h"
#include "Player/NaziZombiePlayerState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABarricade::ABarricade()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	//CollisionMesh->SetupAttachment(MeshComp);

	/*bIsUsed = false;

	Cost = 500;*/
	AccessZone = 0;

	//FString TestPassword = FMD5::HashAnsiString(*FString("HGHHdsfhjJH123"));
	
}


void ABarricade::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
		
	UIMessage += ObjectName + "[Cost: " + FString::FromInt(Cost) + "]";
}




void ABarricade::OnRep_ObjectUsed()
{
	//SetActorEnableCollision(false);
	CollisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!OpenAnimation) return;
	MeshComp->PlayAnimation(OpenAnimation, false);
}


void ABarricade::Use(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player)
	{
		if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
		{
			if (!PState->DecrementPoints(Cost)) return;

			bIsUsed = true;
			OnRep_ObjectUsed();

			if (!GetWorld()) return;
			if (ANaziZombieGameMode* GM = GetWorld()->GetAuthGameMode<ANaziZombieGameMode>())
			{
				GM->NewZoneActive(AccessZone);
			}
		}
	}
	
}