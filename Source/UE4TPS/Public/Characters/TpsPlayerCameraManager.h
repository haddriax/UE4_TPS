// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "TpsPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class UE4TPS_API ATpsPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
private:
	float NormalFOV = 90.0f;
	float AimingFOV = 55.0f;

public:
	ATpsPlayerCameraManager();

private:
	void UpdateFOV(float DeltaTime);

public:
	virtual void UpdateCamera(float DeltaTime) override;
};
