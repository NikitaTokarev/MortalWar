// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/Game/NaziZombieGameMode.h"
#include "NaziZombie/Game/NaziZombiePlayerSpawnPoint.h"
#include "WorldAtWar/Public/Player/CharacterBase.h"
#include "NaziZombie/Game/NaziZombiePlayerSpawnPoint.h"
#include "NaziZombie/Game/NaziZombieZombieSpawnPoint.h"
#include "NaziZombie/Game/NaziZombieGameState.h"
#include "Player/NaziZombieCharacter.h"
#include "NaziZombie/Zombie/ZombieBase.h"
#include "NaziZombie/Useables/Barricade.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


ANaziZombieGameMode::ANaziZombieGameMode()
{
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BlueprintClasses/Player/BP_CharacterBase.BP_CharacterBase_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;*/
	GameStateClass = ANaziZombieGameState::StaticClass();

	bHasLoadedSpawnPoints = false;
	ZombiesRemaining = 0;
}



void ANaziZombieGameMode::BeginPlay()
{
	Super::BeginPlay();

	ZombieGameState = GetGameState<ANaziZombieGameState>();
	//CalculateZombieCount();

	if (!GetWorld()) return;

	TArray<AActor*> TempActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANaziZombieZombieSpawnPoint::StaticClass(), TempActors);
	for (auto& Actor : TempActors)
	{
		if (ANaziZombieZombieSpawnPoint* SpawnPoint = Cast<ANaziZombieZombieSpawnPoint>(Actor))
		{
			if (ABarricade* LinkedBarricade = SpawnPoint->GetLinkedBarricade())
			{
				SpawnPoint->SetZone(LinkedBarricade->GetAccessZone());
				ZombieSpawnPoints.Add(SpawnPoint);
				UE_LOG(LogTemp, Warning, TEXT("Zone Number: %d"), LinkedBarricade->GetAccessZone());
			}
			else
			{
				ActiveZombieSpawnPoints.Add(SpawnPoint);
			}
		}

	}

	const FString MapName = GetWorld()->GetFName().ToString();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *MapName);	


	GetWorld()->GetTimerManager().SetTimer(TZombieSpawnHandle, this, &ANaziZombieGameMode::SpawnZombie, 1.0f, true);
	GetWorld()->GetTimerManager().PauseTimer(TZombieSpawnHandle);
	CalculateZombieCount();
}


void ANaziZombieGameMode::CalculateZombieCount()
{
	if (ZombieGameState)
	{
		if (ZombieGameState->GetTotalZombiesRemaining() >= MaxZombiesOnMap)
		{
			if (TTimesUpHandle.IsValid())
			{
				GetWorldTimerManager().ClearTimer(TTimesUpHandle);
			}
			float Time = FMath::Min(30.0f + ZombieGameState->GetRoundNumber() * 45.0f, 240.0f);
			GetWorldTimerManager().SetTimer(TTimesUpHandle, this, &ANaziZombieGameMode::TimesUp, Time, false);
		

			return;
		}

		uint16 RoundNumber = ZombieGameState->GetRoundNumber();
		uint8 PlayerCount = ZombieGameState->PlayerArray.Num();
		//uint8 MaxZombiesOnMapAtOnce = 14;

		if (PlayerCount > 1)
		{
			ZombieSpawnRatio += PlayerCount * 2;
		}

		if (RoundNumber > 0 && RoundNumber <= 5)
		{
			ZombiesRemaining = FMath::Max(FMath::FloorToInt((RoundNumber * 0.3f) * ZombieSpawnRatio) + FMath::RandRange(1,3), 5);
		}
		else
		{
			ZombiesRemaining = FMath::FloorToInt((RoundNumber * 0.15f) * ZombieSpawnRatio) + FMath::RandRange(0,2);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Zombie: %d"), ZombiesRemaining);

		ZombiesRemaining = FMath::Min(ZombiesRemaining, uint16(MaxZombiesOnMap - ZombieGameState->GetTotalZombiesRemaining()));

		ZombieGameState->SetTotalZombiesRemaining(ZombiesRemaining);
		GetWorld()->GetTimerManager().UnPauseTimer(TZombieSpawnHandle);
	}
}



void ANaziZombieGameMode::SpawnZombie()
{
	if (ZombiesRemaining > 0)
	{
		int RandomIndex = FMath::RandRange(0, ActiveZombieSpawnPoints.Num() - 1);

		if (RandomIndex < 0) return;

		if (auto SpawnPoint = ActiveZombieSpawnPoints[RandomIndex])
		{
			FVector Loc = SpawnPoint->GetActorLocation();
			FRotator Rot = SpawnPoint->GetActorRotation();

			
			if (AZombieBase* Zombie = GetWorld()->SpawnActor<AZombieBase>(GetZombieClass(), Loc, Rot))
			{
				--ZombiesRemaining;
			}
		}
	}
	else
	{
		GetWorld()->GetTimerManager().PauseTimer(TZombieSpawnHandle);

		if (TTimesUpHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(TTimesUpHandle);
		}
		float Time = FMath::Min(30.0f + ZombieGameState->GetRoundNumber() * 45.0f, 240.0f);

		GetWorldTimerManager().SetTimer(TTimesUpHandle, this, &ANaziZombieGameMode::TimesUp, Time, false);
	}	
}



TSubclassOf<class AZombieBase> ANaziZombieGameMode::GetZombieClass() const
{
	uint16 RoundIndex = ZombieGameState->GetRoundNumber() - 1;
	uint16 Index = ZombiesWaves.IsValidIndex(RoundIndex) ? RoundIndex : ZombiesWaves.Num() - 1;

	auto Wave = ZombiesWaves[Index];

	for (const auto& It : Wave.ZombieWave)
	{		
		checkf(It.Key, TEXT("ZombieClass Is Not Valid"));		

		if (FMath::RandRange(0.0f, 1.0f) <= It.Value)
		{			
			return It.Key;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("NazizombieGameMode.cpp::GetZombieClass: Cannot spawn zombie - Invalid chance"));
	return nullptr;
}



void ANaziZombieGameMode::TimesUp()
{
	UE_LOG(LogTemp, Warning, TEXT("TIME'S UP"));

	UE_LOG(LogTemp, Warning, TEXT("ROUND %d FINISHED"), ZombieGameState->GetRoundNumber());
	ZombieGameState->IncrementRoundNumber();

	CalculateZombieCount();
}




void ANaziZombieGameMode::NewZoneActive(uint8 ZoneNumber)
{
	UE_LOG(LogTemp, Warning, TEXT("SETTING ACTIVE ZONE: %d"), ZoneNumber);


	for (int16 x = ZombieSpawnPoints.Num() - 1; x >= 0; --x)
	{
		ANaziZombieZombieSpawnPoint* SpawnPoint = ZombieSpawnPoints[x];

		if (SpawnPoint && ZoneNumber == SpawnPoint->GetZone() && !SpawnPoint->IsActive())
		{
			ActiveZombieSpawnPoints.Add(SpawnPoint);
			SpawnPoint->Activate();

			ZombieSpawnPoints.RemoveAt(x);
		}
	}
}



void ANaziZombieGameMode::ZombieKilled()
{
	if (ZombieGameState)
	{
		ZombieGameState->ZombieKilled();
		UE_LOG(LogTemp, Error, TEXT("Zombie Remaining: %u"), ZombieGameState->GetTotalZombiesRemaining());
		if (ZombieGameState->GetTotalZombiesRemaining() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ROUND %d FINISHED"), ZombieGameState->GetRoundNumber());
			ZombieGameState->IncrementRoundNumber();
			FTimerHandle TempHandle;
			GetWorldTimerManager().SetTimer(TempHandle, this, &ANaziZombieGameMode::CalculateZombieCount, 5.0f, false);

			if (TTimesUpHandle.IsValid())
			{
				GetWorldTimerManager().ClearTimer(TTimesUpHandle);
			}
		}
	}
}





void ANaziZombieGameMode::SetSpawnPoints()
{
	if (!GetWorld()) return;

	TArray<AActor*> TempActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANaziZombiePlayerSpawnPoint::StaticClass(), TempActors);
	if (TempActors.Num())
	{
		for (auto Actor : TempActors)
		{
			if (ANaziZombiePlayerSpawnPoint* SpawnPoint = Cast<ANaziZombiePlayerSpawnPoint>(Actor))
			{
				PlayerSpawnPoints.Add(SpawnPoint);
			}

		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawn Point Count: %d"), PlayerSpawnPoints.Num());
	bHasLoadedSpawnPoints = true;
}




void ANaziZombieGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (bHasLoadedSpawnPoints == false)
	{
		SetSpawnPoints();
	}

	for (auto SpawnPoint : PlayerSpawnPoints)
	{
		if (!SpawnPoint->IsUsed())
		{
			FVector SpawnLocation = SpawnPoint->GetActorLocation();
			if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(PlayerClass, SpawnLocation, SpawnPoint->GetActorRotation()))
			{
				UE_LOG(LogTemp, Warning, TEXT("SPAWNED PAWN TO POSSESS"));
				NewPlayer->Possess(Pawn);
				SpawnPoint->SetUsed(true);
				return;
			}
		}
	}
}



void ANaziZombieGameMode::AddNewZombieSpawnPoints(TArray<ANaziZombieZombieSpawnPoint*> NewSpawnPoints)
{
	for (auto SpawnPoint : NewSpawnPoints)
	{
		SpawnPoint->Activate();

		ZombieSpawnPoints.Add(SpawnPoint);
		ActiveZombieSpawnPoints.Add(SpawnPoint);
	}
}