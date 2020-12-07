// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FireMode/WeaponFireModeComponent.h"
#include "AutoFireMode.generated.h"

/**
 * Shot at a given fire rate until EndFire is called or IsRefiring() become false.
 */
UCLASS()
class UE4TPS_API UAutoFireMode : public UWeaponFireModeComponent
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
	UAutoFireMode();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

	virtual void BurstStarted() override;
	virtual void BurstEnded() override;
	virtual void Shot() override;

	virtual bool IsRefiring() override;
	virtual bool CanFire() override;

public:
};
