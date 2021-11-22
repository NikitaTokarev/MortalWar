// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/OneWayPortal.h"
#include "Player/NaziZombieCharacter.h"

#include "Components/StaticMeshComponent.h"


AOneWayPortal::AOneWayPortal()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	RootComponent = MeshComp;

	Cost = 0;

	Destination.SetScale3D(FVector(1.0f));
}



void AOneWayPortal::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);

	UIMessage += ObjectName + "[Cost: " + FString::FromInt(Cost) + "]";
}


void AOneWayPortal::Use(ANaziZombieCharacter* Player)
{
	Player->SetActorLocationAndRotation(Destination.GetLocation(), Destination.GetRotation());
	OnPortalUsed.Broadcast();
}
