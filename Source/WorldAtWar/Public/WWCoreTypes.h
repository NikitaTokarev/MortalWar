#pragma once

#include "WWCoreTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponID : uint8
{
	Colt1911 UMETA(DisplayName, "1911"),
	M1Carbine UMETA(DisplayName, "M1Carbine"),
	STG44 UMETA(DisplayName, "STG44")
};