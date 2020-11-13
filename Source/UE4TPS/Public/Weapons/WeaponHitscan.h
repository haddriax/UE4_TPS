// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"

#include "../GameTypes.h"

#include "WeaponHitscan.generated.h"

USTRUCT()
struct FWeaponHitScanConfigs
{
	GENERATED_BODY();

	/*
	* Range limit for full damage to apply.
	*/
	UPROPERTY(EditDefaultsOnly, Category = HitScan)
		float OptimalRange;
	/*
	* Range limit for minimal damage to apply.
	*/
	UPROPERTY(EditDefaultsOnly, Category = HitScan)
		float MaxRange;
	/*
	* Maximal amount of damage, dealt when <= OptimalRange.
	*/
	UPROPERTY(EditDefaultsOnly, Category = HitScan)
		float OptimalDamage;
	/*
	* Minimal amount of damage, dealt when OptimalRange < Range <= MaxRange.
	*/
	UPROPERTY(EditDefaultsOnly, Category = HitScan)
		float MinDamage;

	FWeaponHitScanConfigs()
	{
		OptimalRange = 60000.f;
		MaxRange = 100000.f;
		OptimalDamage = 10.f;
		MinDamage = 2.f;
	}
};

/*
* Data collection for a shot fired by any hitscan weapon.
* Is used to provide information to the hit actor.
*/
USTRUCT()
struct FHitscanShot
{
	GENERATED_BODY();

	AActor* OriginWeapon = nullptr;

	FVector OriginLocation;
	FVector EndLocation;
	FVector Direction;

	int32 RandomSeed;

	/*
	* Time when this shot was fired.
	*/
	float ShotGameTime;

	float Damage;
	TSubclassOf<UDamageType> DamageType;

	FHitscanShot()
	{
		OriginWeapon = nullptr;
		OriginLocation = FVector::ZeroVector;
		EndLocation = FVector::ZeroVector;
		Direction = FVector::ZeroVector;

		RandomSeed = 0;
		ShotGameTime = -1.0f;
		Damage = 1.0f;
	}

	FHitscanShot(AActor* _OriginWeapon, FVector _OriginLocation, FVector _Direction, float _Damage, TSubclassOf<UDamageType> _DamageType)
	{
		OriginWeapon = _OriginWeapon;
		OriginLocation = _OriginLocation;
		Direction = _Direction;
		Damage = _Damage;
		DamageType = _DamageType;
	}
};

/*
* Struct containing information on the Hit, Shot, Damages, Player and Weapon.
*/
USTRUCT()
struct FHitscanHit
{
	GENERATED_BODY();

	/*
	* The shot that made this hit.
	*/
	FHitscanShot Shot;
	FHitResult HitResult;
	AWeaponHitscan* Instigator;

	FHitscanHit()
	{

	}
};

/**
 * Hitscan weapon Trace and handle impact in the same frame, making an instant hit.
 */
UCLASS()
class UE4TPS_API AWeaponHitscan : public AWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Configs")
		FWeaponHitScanConfigs HitscanConfigs;

	UPROPERTY(EditAnywhere)
		bool bDrawRay = false;

	UPROPERTY(EditAnywhere)
		float DebugTraceLifeSpan = 8.0f;

private:
	void DrawDebugTrace(const FVector& Begin, const FVector& End, FColor Color = FColor::Blue);

protected:
	virtual void FireWeapon() override;

	/*
	* Hydrate TraceStart and TraceEnd for the TraceFromCamera of this frame.
	* @param OUT FVector : TraceStart computed and set here.
	* @param OUT FVector : TraceEnd computed and set here.
	*/
	void PrepareTraceFromCamera(FVector& OutTraceStart, FVector& OutTraceEnd);
	/*
	* Direct Trace from Camera center. Used to know where the player want to shoot, then trace from Muzzle.
	*/
	FHitResult TraceFromCamera(FVector& OutTraceStart, FVector& OutTraceEnd);

	/*
	* Hydrate TraceStart and TraceEnd for the weapon LineTrace of this frame, applying Spread if !bIgnoreSpread.
	* @param OUT FVector : TraceStart computed and set here.
	* @param OUT FVector : TraceEnd computed and set here.
	* @return FHitscanShot :The resulting shot datas.
	*/
	FHitscanShot PrepareMuzzleTrace(FVector& OutTraceStart, FVector& OutTraceEnd, bool bIgnoreSpread = false);
	const FHitscanHit TraceFromMuzzle(FHitscanShot Shot);


	/*
	* Line Trace from the weapon.
	* @param : FHitscanShot ref hydrated with the Trace datas.
	* @return : The hydrated FHitscanHit struct.
	*/
	const FHitscanHit TraceWeapon();

	/*
	* Handle the FHitscanHit resulting from TraceWeapon.
	* @param : ref to the hit datas struct.
	*/
	void HandleHit(const FHitscanHit& HitDatas);

};
