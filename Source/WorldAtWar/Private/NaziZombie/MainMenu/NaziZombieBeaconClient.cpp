// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/MainMenu/NaziZombieBeaconClient.h"



ANaziZombieBeaconClient::ANaziZombieBeaconClient()
{
	PlayerIndex = 0;
}




void ANaziZombieBeaconClient::OnFailure()
{
	UE_LOG(LogTemp, Warning, TEXT("CLIENT FAILED TO CONNECT TO HOST BEACON"));
	FOnConnected.Broadcast(false);
}

void ANaziZombieBeaconClient::OnConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("CLIENT CONNECTED TO HOST BEACON"));
	FOnConnected.Broadcast(true);
}



void ANaziZombieBeaconClient::SendChatMessage(const FText& ChatMessage)
{	
	Server_SendChatMessage(ChatMessage);
}

bool ANaziZombieBeaconClient::Server_SendChatMessage_Validate(const FText& ChatMessage)
{
	return true;
}

void ANaziZombieBeaconClient::Server_SendChatMessage_Implementation(const FText& ChatMessage)
{
	FString Message = PlayerName + ": " + ChatMessage.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Chat: %s"), *Message);

	if (ANaziZombieBeaconHostObject* Host = Cast<ANaziZombieBeaconHostObject>(BeaconOwner))
	{
		Host->SendChatToLobby(FText::FromString(Message));
	}
}

void ANaziZombieBeaconClient::Client_OnChatMessageReceived_Implementation(const FText& ChatMessage)
{
	FOnChatReceived.Broadcast(ChatMessage);
}



void ANaziZombieBeaconClient::Client_OnDisconnected_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("DISCONNECTED"));
	FOnDisconnected.Broadcast();
}

void ANaziZombieBeaconClient::Client_OnLobbyUpdated_Implementation(FNaziZombieLobbyInfo LobbyInfo)
{
	FOnLobbyUpdated.Broadcast(LobbyInfo);
}




void ANaziZombieBeaconClient::Client_FullConnect_Implementation()
{	
	FOnFullConnect.Broadcast();
}

void ANaziZombieBeaconClient::FullConnectToServer(const FString& JoinAddress)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);		
	}
	LeaveLobby();
}

void ANaziZombieBeaconClient::SetPlayerIndex(uint8 Index)
{
	PlayerIndex = Index;
}

uint8 ANaziZombieBeaconClient::GetPlayerIndex()
{
	return PlayerIndex;
}

void ANaziZombieBeaconClient::SetPlayerName(const FString& NewPlayerName)
{
	PlayerName = NewPlayerName;
}

FString ANaziZombieBeaconClient::GetPlayerName()
{
	return PlayerName;;
}

bool ANaziZombieBeaconClient::ConnectToServer(const FString& Address, const FString& NewPlayerName)
{
	FURL Destination = FURL(nullptr, *Address, ETravelType::TRAVEL_Absolute);
	Destination.Port = 7787; // ********************************* From DefaultEngine.ini **********************************	

	return InitClient(Destination);
}


void ANaziZombieBeaconClient::LeaveLobby()
{
	DestroyBeacon();
}








void ANaziZombieBeaconClient::RegisterNewPlayer(const FString& NewPlayerName)
{	
	Server_RegisterNewPlayer(NewPlayerName);
}



bool ANaziZombieBeaconClient::Server_RegisterNewPlayer_Validate(const FString& NewPlayerName)
{
	return true;
}



void ANaziZombieBeaconClient::Server_RegisterNewPlayer_Implementation(const FString& NewPlayerName)
{
	if (ANaziZombieBeaconHostObject* Host = Cast<ANaziZombieBeaconHostObject>(BeaconOwner))
	{
		Host->RegisterNewPlayer(this, NewPlayerName);
	}
}