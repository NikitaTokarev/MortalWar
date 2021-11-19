// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NaziZombie/Game/NaziZombieGameInstanceBase.h"
#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "Http.h"
#include "NaziZombieBeaconHostObject.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FNaziZombieLobbyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* MapImage;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> PlayerList;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHostLobbyUpdated, FNaziZombieLobbyInfo, FOnHostLobbyUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHostChatReceived, const FText&, FOnHostChatReceived);

UCLASS()
class WORLDATWAR_API ANaziZombieBeaconHostObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()
	
public:
	ANaziZombieBeaconHostObject();
	
protected:
	FHttpModule* Http;
	int ServerID;
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	

protected:
	FServerData ServerData;

	FNaziZombieLobbyInfo LobbyInfo;

	UFUNCTION(BlueprintCallable)
	void SetServerData(FServerData NewServerData);

	UFUNCTION(BlueprintCallable)
	void UpdateServerData(FServerData NewServerData);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetCurrentPlayerCount() const;

	UPROPERTY(BlueprintAssignable)
	FHostLobbyUpdated FOnHostLobbyUpdated;

	UPROPERTY(BlueprintAssignable)
	FHostChatReceived FOnHostChatReceived;
	
	FTimerHandle TInitialLobbyHandle;
	void InitialLobbyHandling();

	UFUNCTION(BlueprintCallable)
	void UpdateLobbyInfo(FNaziZombieLobbyInfo NewLobbyInfo);

	void UpdateClientLobbyInfo();


public:

	void RegisterNewPlayer(class ANaziZombieBeaconClient* NewClientActor, const FString& NewPlayerName);
	
			

protected:
	virtual void BeginPlay() override;

	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;
	virtual void NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor) override;

	UFUNCTION(BlueprintCallable)
	void ShutdownServer();

	void DisconnectAllClients();

	virtual void DisconnectClient(AOnlineBeaconClient* ClientActor) override;

	UFUNCTION(BlueprintCallable)
	void StartServer(const FString& MapURL);

public:
	UFUNCTION(BlueprintCallable)
	void SendChatToLobby(const FText& ChatMessage);
};
