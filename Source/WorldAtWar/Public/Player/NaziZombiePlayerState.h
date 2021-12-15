// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NaziZombiePlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPointsChanged, int32, Points);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKnifing, class AZombieBase*, SlainEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeadshot);


UCLASS()
class WORLDATWAR_API ANaziZombiePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANaziZombiePlayerState();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PointsChanged, EditDefaultsOnly)
	int32 Points;

	UPROPERTY(Replicated)
	int32 Kills;

	UPROPERTY(Replicated)
	int32 Headshots;

	UPROPERTY(Replicated)
	int32 Knifings;

	UPROPERTY(BlueprintAssignable)
	FPointsChanged OnPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHeadshot OnHeadshot;

	UPROPERTY(BlueprintAssignable)
	FOnKnifing OnKnifing;

	UFUNCTION()		
	void OnRep_PointsChanged();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPoints() const { return Points; }

	void IncrementPoints(int16 Value);
	bool DecrementPoints(int16 Value);

	void AddKill() { ++Kills; }
	void AddHeadshot();
	void AddKnifing(class AZombieBase* SlainEnemy);

	UFUNCTION(BlueprintCallable)
	int32 GetTotalScore() const;

	UFUNCTION(BlueprintCallable)
	void GetBattleStats(int32& ResKills, int32& ResHeadshots, int32& ResKnifings, int32& ResPoints) const;
};
