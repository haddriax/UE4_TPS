// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FireMode/WeaponFireModeComponent.h"
#include "SingleFireMode.generated.h"

/**
 * Shot one time per input, if possible.
 */
UCLASS()
class UE4TPS_API USingleFireMode : public UWeaponFireModeComponent
{
	GENERATED_BODY()

protected:
	FTimerHandle TH_Refire;

	/*
	* Time between each shot.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float TimeBetweenShots;

public:
	// Sets default values for this component's properties
	USingleFireMode();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

	virtual void BurstStarted() override;
	virtual void BurstEnded() override;
	virtual void Shot() override;

	virtual bool CanFire() override;

public:
};
