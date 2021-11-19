// Fill out your copyright notice in the Description page of Project Settings.


#include "NaziZombie/MainMenu/NaziZombieMainMenuGameMode.h"
#include "NaziZombie/MainMenu/NaziZombieBeaconHostObject.h"

#include "OnlineBeaconHost.h"


ANaziZombieMainMenuGameMode::ANaziZombieMainMenuGameMode()
{
	Host = nullptr;
	HostObject = nullptr;
}

bool ANaziZombieMainMenuGameMode::CreateHostBeacon()
{
	Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass());
	if (Host)
	{
		if (Host->InitHost())
		{
			Host->PauseBeaconRequests(false);

			HostObject = GetWorld()->SpawnActor<ANaziZombieBeaconHostObject>(ANaziZombieBeaconHostObject::StaticClass());
			if (HostObject)
			{
				Host->RegisterHost(HostObject);
				return true;
			}
		}
	}

	return false;
}

ANaziZombieBeaconHostObject* ANaziZombieMainMenuGameMode::GetBeaconHost()
{
	return HostObject;
}


