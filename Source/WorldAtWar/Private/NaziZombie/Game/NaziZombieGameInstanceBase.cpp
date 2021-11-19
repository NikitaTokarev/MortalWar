// Fill out your copyright notice in the Description page of Project Settings.

#include "NaziZombie/Game/NaziZombieGameInstanceBase.h"


UNaziZombieGameInstanceBase::UNaziZombieGameInstanceBase()
{
	Http = &FHttpModule::Get();	
}

TArray<FServerData>& UNaziZombieGameInstanceBase::GetServerList()
{
	return ServerList;
}

void UNaziZombieGameInstanceBase::GenerateServerList()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &UNaziZombieGameInstanceBase::OnServerListRequestComplete);

	Request->SetURL("https://localhost:44376/api/Host");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->ProcessRequest();
}

void UNaziZombieGameInstanceBase::OnServerListRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	ServerList.Empty();
	if (Success)
	{
		FString ResponseStr = Response->GetContentAsString();
		ResponseStr.InsertAt(0, FString("{\"Response\":"));
		ResponseStr.AppendChar('}');
		UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseStr);

		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseStr);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			TArray<TSharedPtr<FJsonValue>> JsonValues = JsonObject->GetArrayField(TEXT("Response"));			

			for (TSharedPtr<FJsonValue> Value : JsonValues)
			{
				FServerData ServerData = FServerData();
				TSharedPtr<FJsonObject> JsonObj = Value->AsObject();

				ServerData.IPAddress = JsonObj->GetStringField("IPAddress");
				ServerData.ServerName = JsonObj->GetStringField("ServerName");
				ServerData.MapName = JsonObj->GetStringField("MapName");
				ServerData.CurrentPlayers = JsonObj->GetIntegerField("CurrentPlayers");
				ServerData.MaxPlayers = JsonObj->GetIntegerField("MaxPlayers");

				ServerList.Add(ServerData);
				
			}

			FOnServerReceived.Broadcast();
			/*for (auto ServerData : ServerList)
			{				
				UE_LOG(LogTemp, Warning, TEXT("IP: %s"), *ServerData.IPAddress);
				UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *ServerData.ServerName);
				UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *ServerData.MapName);
				UE_LOG(LogTemp, Warning, TEXT("CurrentPlayers: %d"), ServerData.CurrentPlayers);
				UE_LOG(LogTemp, Warning, TEXT("MaxPlayers: %d"), ServerData.MaxPlayers);

				UE_LOG(LogTemp, Warning, TEXT("END OF SERVER ENTRY"));
			}*/
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GETSERVERLIST FAILED"));
	}
}

void UNaziZombieGameInstanceBase::GoToMap(const FString& MapURL)
{
	if (!GetWorld()) return;
	GetWorld()->ServerTravel(MapURL);
}
