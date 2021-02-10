// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FireMode/SingleFireMode.h"

USingleFireMode::USingleFireMode()
	: Super()
{
	TimeBetweenShots = 0.3f;
}

void USingleFireMode::BeginPlay()
{
	Super::BeginPlay();
}

void USingleFireMode::InitializeComponent()
{
	Super::InitializeComponent();
}

void USingleFireMode::BurstStarted()
{
	Super::BurstStarted();

	const float GameTime = GetWorld()->GetTimeSeconds();
	if (CanFire())
	{
		// If LastFireTime && TimeBetweenShots are valid, and the TimeBetweenShots is NOT elapsed.
		if (LastFireTime > 0 && TimeBetweenShots > 0.0f && (LastFireTime + TimeBetweenShots > GameTime))
		{			
		}
		// Ready to Shot.
		else
		{
			Shot();
		}
	}
	BurstEnded();
}

void USingleFireMode::BurstEnded()
{
	Super::BurstEnded();
}

void USingleFireMode::Shot()
{
	Super::Shot();

	// Keep the last fire time, so we know if we can shot again at a given frame.
	LastFireTime = GetWorld()->GetTimeSeconds();
}


bool USingleFireMode::CanFire()
{
	return Super::CanFire();
}
