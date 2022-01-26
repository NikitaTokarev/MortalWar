// Fill out your copyright notice in the Description page of Project Settings.

#include "NaziZombie/Useables/PrayerPlace.h"
#include "Components/BoxComponent.h"


APrayerPlace::APrayerPlace()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>("CollisionComponent");
	SetRootComponent(CollisionComp);
}



bool APrayerPlace::MightPray_Implementation(ANaziZombieCharacter* Player)
{
	return true;
}


void APrayerPlace::OnRep_ObjectUsed()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnObjectUsed.Broadcast();

	SetLifeSpan(1.0f);
}



void APrayerPlace::Use(ANaziZombieCharacter* Player)
{
	if (Player && MightPray(Player))
	{
		bIsUsed = true;
		OnRep_ObjectUsed();
	}
	else
	{
		OnFailUse.Broadcast();
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (!GetWorld()) return;

		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &APrayerPlace::AllowPraying, 1.0f, false);
	}
}



void APrayerPlace::AllowPraying()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
