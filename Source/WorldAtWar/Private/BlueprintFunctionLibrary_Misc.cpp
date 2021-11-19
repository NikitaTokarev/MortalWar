// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionLibrary_Misc.h"
#include "Sound/SoundClass.h"


void UBlueprintFunctionLibrary_Misc::SetSoundClassVolume(USoundClass* SoundClass, float Volume)
{
	if (!SoundClass) return;

	SoundClass->Properties.Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
}


void UBlueprintFunctionLibrary_Misc::ToggleSoundClassVolume(USoundClass* SoundClass, bool bIsOn)
{
	if (!SoundClass) return;

	SoundClass->Properties.Volume = bIsOn ? 1.0f : 0.0f;
}
