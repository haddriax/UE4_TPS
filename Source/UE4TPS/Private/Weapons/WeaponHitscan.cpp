// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/WeaponHitscan.h"

#include "Characters/TpsCharacterBase.h"

#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/HitResponsiveInterface.h"
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
	const FVector TraceStart = GetMuzzleWorldLocation();
	const FVector TraceEnd = GetActorRightVector() * HitscanConfigs.MaxRange;

	FCollisionQueryParams ColQueryParams;
	FCollisionResponseParams ColResParams;

	FHitscanHit hit;

	// Ignore self and the holder for Linetrace.
	ColQueryParams.AddIgnoredActor(this->GetUniqueID());
	ColQueryParams.AddIgnoredActor(GetParentCharacter()->GetUniqueID());

	// Trace here.
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		hit.HitResult,
		TraceStart,
		TraceEnd, //GetActorLocation() + GetActorForwardVector() * 1000,
		ECollisionChannel::ECC_MAX, // Context
		ColQueryParams,
		ColResParams
	);

	if (bDrawRay)
	{
		if (bIsHit)
		{
			DrawDebugTrace(TraceStart, hit.HitResult.ImpactPoint, FColor::Green);
			DrawDebugTrace(hit.HitResult.ImpactPoint, TraceEnd, FColor::Red);
		}
		else
		{
			DrawDebugTrace(TraceStart, TraceEnd, FColor::Red);
		}
	}

	AActor* actorHit = hit.HitResult.GetActor();

	if (actorHit)
	{		
		if (actorHit->GetClass()->ImplementsInterface(UHitResponsiveInterface::StaticClass()))
		{
			CastChecked<IHitResponsiveInterface>(actorHit)->OnHit(this);
		}	
	}
}