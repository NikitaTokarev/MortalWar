// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Http.h"
#include "NaziZombieGameInstanceBase.generated.h"

class UTexture2D;

//USTRUCT(BlueprintType)
//struct FMapInfo
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FString MapURL;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FString MapName;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FString MapDescription;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	UTexture2D* MapImage;
//};

USTRUCT(BlueprintType)
struct FServerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ServerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IPAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ServerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxPlayers;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FServerReceived);

UCLASS()
class WORLDATWAR_API UNaziZombieGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UNaziZombieGameInstanceBase();


protected:
	FHttpModule* Http;

	TArray<FServerData> ServerList;

	UFUNCTION(BlueprintCallable)
	TArray<FServerData>& GetServerList();

	UPROPERTY(BlueprintAssignable)
	FServerReceived FOnServerReceived;

	UPROPERTY(BlueprintReadWrite)
	FString LocalPlayerName;

	UFUNCTION(BlueprintCallable)
	void GenerateServerList();

	void OnServerListRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);

protected:
	UFUNCTION(BlueprintCallable)
	void GoToMap(const FString& MapURL);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FString& GetPlayerName() const { return LocalPlayerName; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& NewPlayerName) { LocalPlayerName = NewPlayerName; }
	
};
