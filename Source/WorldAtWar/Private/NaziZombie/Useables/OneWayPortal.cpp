// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/OneWayPortal.h"
#include "Player/NaziZombieCharacter.h"
#include "GameFramework/Controller.h"

#include "Components/StaticMeshComponent.h"


AOneWayPortal::AOneWayPortal()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	RootComponent = MeshComp;

	ObjectName = FText::FromString("Portal");
	Cost = 0;	
}



void AOneWayPortal::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);

	if (Cost != 0)
	{
		const FString ResultMessage = UIMessage.ToString() + ObjectName.ToString() + "[Cost: " + FString::FromInt(Cost) + "]";

		UIMessage = FText::FromString(ResultMessage);
	}
	else
	{
		const FString ResultMessage = "Two-finger touch to use " + ObjectName.ToString();

		UIMessage = FText::FromString(ResultMessage);
	}
		
}


void AOneWayPortal::Use(ANaziZombieCharacter* Player)
{
	Player->SetActorLocation(DestinationLocation);	
	Player->GetController()->SetControlRotation(DestinationRotation);	

	OnPortalUsed.Broadcast();
}
