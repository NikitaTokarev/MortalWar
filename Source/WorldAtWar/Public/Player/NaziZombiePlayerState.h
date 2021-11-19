// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NaziZombiePlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPointsChanged, int32, Points);


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

	UFUNCTION()		
	void OnRep_PointsChanged();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPoints() const { return Points; }

	void IncrementPoints(int16 Value);
	bool DecrementPoints(int16 Value);

	void AddKill() { ++Kills; }
	void AddHeadshot() { ++Headshots; }
	void AddKnifing() { ++Knifings; }

	UFUNCTION(BlueprintCallable)
	int32 GetTotalScore() const;

	UFUNCTION(BlueprintCallable)
	void GetBattleStats(int32& ResKills, int32& ResHeadshots, int32& ResKnifings, int32& ResPoints) const;
};
