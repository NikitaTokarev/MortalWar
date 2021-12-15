// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NaziZombiePlayerState.h"

#include "Net/UnrealNetwork.h"



ANaziZombiePlayerState::ANaziZombiePlayerState()
{
	Points = 500;

	Kills = 0;
	Headshots = 0;
	Knifings = 0;
}



void ANaziZombiePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANaziZombiePlayerState, Points);
	DOREPLIFETIME(ANaziZombiePlayerState, Kills);
	DOREPLIFETIME(ANaziZombiePlayerState, Headshots);
	DOREPLIFETIME(ANaziZombiePlayerState, Knifings);
}


void ANaziZombiePlayerState::OnRep_PointsChanged()
{
	OnPointsChanged.Broadcast(Points);
}


void ANaziZombiePlayerState::IncrementPoints(int16 Value)
{
	Points += Value;
	if(HasAuthority()) 
		OnRep_PointsChanged();
	OnPointsChanged.Broadcast(Points);
	UE_LOG(LogTemp, Warning, TEXT("POINTS: %d"), Points);
}


bool ANaziZombiePlayerState::DecrementPoints(int16 Value)
{
	if (HasAuthority())
	{
		if (Points - Value < 0)
		{
			return false;
		}
		else
		{
			Points -= Value;
			OnRep_PointsChanged();
			return true;
		}
	}
	return false;
}


void ANaziZombiePlayerState::AddHeadshot()
{
	++Headshots;
	OnHeadshot.Broadcast();
}


void ANaziZombiePlayerState::AddKnifing(class AZombieBase* SlainEnemy)
{
	++Knifings;
	OnKnifing.Broadcast(SlainEnemy);
}



int32 ANaziZombiePlayerState::GetTotalScore() const
{
	return (Kills * 10) + (Headshots * 50) + (Knifings * 40) + (Points / 2);
}



void ANaziZombiePlayerState::GetBattleStats(int32& ResKills, int32& ResHeadshots, int32& ResKnifings, int32& ResPoints) const
{
	ResKills = this->Kills;
	ResHeadshots = this->Headshots;
	ResKnifings = this->Knifings;
	ResPoints = this->Points;
}
