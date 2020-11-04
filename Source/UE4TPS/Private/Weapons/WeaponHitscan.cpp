// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponHitscan.h"

#include "Characters/TpsCharacter.h"

#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
// #include "Effects/WeaponImpactEffect.h"

void AWeaponHitscan::DrawDebugTrace(const FVector& Begin, const FVector& End, FColor Color)
{
	if (bDrawRay)
	{
		DrawDebugLine(
			GetWorld(),
			Begin,
			End,
			Color,
			false,
			DebugTraceLifeSpan
		);
	}
}

void AWeaponHitscan::FireWeapon()
{
	FVector traceStart, traceEnd;
	FHitscanShot shot = PrepareMuzzleTrace(traceStart, traceEnd, false);
	FHitscanHit hit = TraceFromMuzzle(shot);

	HandleHit(hit);
}

void AWeaponHitscan::PrepareTraceFromCamera(FVector& TraceStart, FVector& TraceEnd)
{
	// Get the direction and the location from the Camera.
	const UCameraComponent* traceCamera = GetParentCharacter()->GetThirdPersonCamera();

	FVector trace_direction = traceCamera->GetForwardVector();

	TraceStart = traceCamera->GetComponentLocation();
	TraceEnd = TraceStart + trace_direction * HitscanConfigs.MaxRange;
}

FHitResult AWeaponHitscan::TraceFromCamera(FVector& TraceStart, FVector& TraceEnd)
{
	FCollisionQueryParams ColQueryParams;
	FCollisionResponseParams ColResParams;

	FHitResult hit;

	// Ignore self and the holder for linetrace.
	ColQueryParams.AddIgnoredActor(this->GetUniqueID());
	ColQueryParams.AddIgnoredActor(GetParentCharacter()->GetUniqueID());

	// Trace here.
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		hit,
		TraceStart,
		TraceEnd,
		ECollisionChannel::ECC_Visibility, // Context
		ColQueryParams,
		ColResParams
	);

	if (bDrawRay)
	{
		DrawDebugTrace(TraceStart, TraceEnd, FColor::Black);
	}
	
	return hit;
}

FHitscanShot AWeaponHitscan::PrepareMuzzleTrace(FVector& TraceStart, FVector& TraceEnd, bool bIgnoreSpread /* = false */)
{
	// Get the hit where the player expect to shoot, i.e. from the camera center.
	PrepareTraceFromCamera(TraceStart, TraceEnd);
	FHitResult fromCameraHit = TraceFromCamera(TraceStart, TraceEnd);

	int32 shotRandomSeed = 0;

	TraceStart = GetMuzzleWorldLocation();
	// Get the impact location, else keep the furthest point.
	TraceEnd = fromCameraHit.bBlockingHit ? fromCameraHit.ImpactPoint : TraceEnd;

	// The trace direction is normalized vector between muzzle and impact point.
	FVector trace_direction = (TraceEnd - TraceStart);
	trace_direction.Normalize();

	// Default is not ignoring spread.
	if (!bIgnoreSpread)
	{
		// Add the spread to the trace direction.
		shotRandomSeed = AddRandomDirectionFromCone(/*OUT*/ trace_direction, CurrentSpread, CurrentSpread, false);
	}

	// Hydrate the Shot struct.
	FHitscanShot hitScanShot;
	hitScanShot.ShotGameTime = GetWorld()->GetTimeSeconds();
	hitScanShot.OriginLocation = TraceStart;
	hitScanShot.EndLocation = TraceEnd;
	hitScanShot.Direction = trace_direction;
	hitScanShot.OriginWeapon = this;
	hitScanShot.RandomSeed = shotRandomSeed;

	return hitScanShot;
}

const FHitscanHit AWeaponHitscan::TraceFromMuzzle(FHitscanShot Shot)
{
	FCollisionQueryParams ColQueryParams;
	FCollisionResponseParams ColResParams;

	FHitscanHit hit;
	hit.Shot = Shot;

	// Ignore self and the holder for linetrace.
	ColQueryParams.AddIgnoredActor(this->GetUniqueID());
	ColQueryParams.AddIgnoredActor(GetParentCharacter()->GetUniqueID());

	// Trace here.
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		hit.HitResult,
		Shot.OriginLocation,
		Shot.EndLocation,
		ECollisionChannel::ECC_Visibility, // Context
		ColQueryParams,
		ColResParams
	);

	if (bDrawRay)
	{
		if (bIsHit)
		{
			DrawDebugTrace(Shot.OriginLocation, hit.HitResult.ImpactPoint, FColor::Green);
			DrawDebugTrace(hit.HitResult.ImpactPoint, Shot.EndLocation, FColor::Red);
		}
		else
		{
			DrawDebugTrace(Shot.OriginLocation, Shot.EndLocation, FColor::Red);
		}
	}

	return hit;
}

void AWeaponHitscan::HandleHit(const FHitscanHit& HitDatas)
{
	if (HitDatas.HitResult.bBlockingHit == true)
	{
		AActor* actorHit = HitDatas.HitResult.GetActor();

		if (actorHit)
		{
			// Do Something ...
		}
	}
}