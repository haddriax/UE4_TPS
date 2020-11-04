#pragma once

#include "GameTypes.generated.h"

UENUM()
namespace EPhysicMaterial
{
	enum Type
	{
		Unknown,
		Concrete,
		Dirt,
		Water,
		Metal,
		Wood,
		Grass,
		Glass,
		Flesh,
	};
}

#define SURFACE_Default		SurfaceType_Default
#define SURFACE_Concrete	SurfaceType1
#define SURFACE_Dirt		SurfaceType2
#define SURFACE_Water		SurfaceType3
#define SURFACE_Metal		SurfaceType4
#define SURFACE_Wood		SurfaceType5
#define SURFACE_Grass		SurfaceType6
#define SURFACE_Glass		SurfaceType7
#define SURFACE_Flesh		SurfaceType8

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Undefined		UMETA(DisplayName = "Undefined"),
	Rifle			UMETA(DisplayName = "Rifle"),
	Pistol			UMETA(DisplayName = "Pistol"),
	Shotgun			UMETA(DisplayName = "Shotgun"),
	// RocketLauncher	UMETA(DisplayName = "RocketLauncher"),
	// GrenadeLauncher	UMETA(DisplayName = "GrenadeLauncher"),
	// SniperRifle		UMETA(DisplayName = "SniperRifle"),
	// Knife			UMETA(DisplayName = "Knife")
};