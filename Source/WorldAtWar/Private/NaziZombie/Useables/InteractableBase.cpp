// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/InteractableBase.h"
#include "Player/NaziZombieCharacter.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;	

	ObjectName = FText::FromString("Object");
	UIMessage = FText::FromString("Two-finger touch to interact with ");	

	bIsUsed = false;

	Cost = 500;
}

void AInteractableBase::OnRep_ObjectUsed()
{

}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();	

	SetReplicates(true);



	//const FString ResultMessage = UIMessage.ToString() + ObjectName.ToString() + "[Cost: " + FString::FromInt(Cost) + "]";

	//UIMessage.FromString(ResultMessage);
}

void AInteractableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableBase, bIsUsed);
}


void AInteractableBase::Use(ANaziZombieCharacter* Player)
{
	
}



