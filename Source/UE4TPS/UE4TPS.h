// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

#include "Characters/TpsCharacterBase.h"
#include "Characters/Controllers/TpsPlayerController.h"
#include "Characters/Components/CharacterWeaponComponent.h"
#include "Weapons/ModularWeapon.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTPS, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogTpsWeapon, Log, All);

#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PICKUP		ECC_GameTraceChannel3