// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "BlueprintFunctionLibrary_Misc.generated.h"



UCLASS()
class WORLDATWAR_API UBlueprintFunctionLibrary_Misc : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SetSoundClassVolume(class USoundClass* SoundClass, float Volume);

	UFUNCTION(BlueprintCallable)
	static void ToggleSoundClassVolume(class USoundClass* SoundClass, bool bIsOn);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsOnline(AActor* Object)
	{
		if (!Object || !Object->GetWorld()) return false;

		return (Object->GetWorld()->GetGameState()->PlayerArray.Num() > 1);
	}	
};
