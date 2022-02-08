// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Useables/AncietBook.h"
#include "Player/NaziZombieCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"



AAncietBook::AAncietBook()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>("CollisionComponent");
	SetRootComponent(CollisionComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComp->SetupAttachment(CollisionComp);
}


void AAncietBook::Use(ANaziZombieCharacter* Player)
{
	bIsUsed = true;
	OnRep_ObjectUsed();

	OnObjectUsed.Broadcast();
}

void AAncietBook::OnRep_ObjectUsed()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//UseObject();
}


//void AAncietBook::UseObject_Implementation()
//{
//
//}
