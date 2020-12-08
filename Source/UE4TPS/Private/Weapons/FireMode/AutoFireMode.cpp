// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FireMode/AutoFireMode.h"

UAutoFireMode::UAutoFireMode()
	: Super()
{
	LastFireTime = 0.0f;
	TimeBetweenShots = 0.2f;
}

void UAutoFireMode::BeginPlay()
{
	Super::BeginPlay();
}

void UAutoFireMode::InitializeComponent()
{
	Super::InitializeComponent();
}

void UAutoFireMode::BurstStarted()
{
	Super::BurstStarted();

	const float GameTime = GetWorld()->GetTimeSeconds();

	// If LastFireTime && TimeBetweenShots are valid, and the TimeBetweenShots is NOT elapsed.
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f && (LastFireTime + TimeBetweenShots > GameTime))
	{
		// Set a non looping Timer which will manage firing when ready.
		GetOwner()->GetWorldTimerManager().SetTimer(TH_Refire,
			this,
			&UAutoFireMode::Shot,
			LastFireTime + TimeBetweenShots - GameTime,
			false);

	}
	// Ready to Shot.
	else
	{
		Shot();
	}
}

void UAutoFireMode::BurstEnded()
{
	Super::BurstEnded();

	GetOwner()->GetWorldTimerManager().ClearTimer(TH_Refire);
}

void UAutoFireMode::Shot()
{
	Super::Shot();

	if (CanFire())
	{
		ShotImpl();

		// Broadcast the OnShot Event.
		if (OnShot.IsBound())
		{
			OnShot.Broadcast();
		}

		Shot_Blueprint();

		if (IsRefiring())
		{
			GetOwner()->GetWorldTimerManager().SetTimer(TH_Refire,
				this,
				&UAutoFireMode::Shot,
				TimeBetweenShots,
				false
			);
		}
	}
	/*
	// Can't shoot in the middle of a burst.
	else if (BurstCounter > 0)
	{
		// End the burst if we cannot fire anymore (ex : out of ammo).
		BurstEnded();
	}
	*/

	// Keep the last fire time, so we know if we can shot again at a given frame.
	LastFireTime = GetWorld()->GetTimeSeconds();
}

bool UAutoFireMode::IsRefiring()
{
	return Super::IsRefiring();
}

bool UAutoFireMode::CanFire()
{
	return Super::CanFire();
}