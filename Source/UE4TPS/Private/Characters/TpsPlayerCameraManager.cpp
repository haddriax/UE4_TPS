// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TpsPlayerCameraManager.h"

#include "Characters/TpsPlayerCharacter.h"
#include "Characters/Controllers/TpsPlayerController.h"

ATpsPlayerCameraManager::ATpsPlayerCameraManager()
{
	bAlwaysApplyModifiers = true;
}

void ATpsPlayerCameraManager::UpdateFOV(float DeltaTime)
{
		/*
	AGameCharacterBase* tpsCharacter = PCOwner ? Cast<AGameCharacterBase>(PCOwner->GetPawn()) : nullptr;

	if (tpsCharacter)
	{
		const float targetedFOV = tpsCharacter->GetTpsCharacterStatesComponent()->IsAiming() ? AimingFOV : NormalFOV;
		const float finalFov = FMath::FInterpTo(LockedFOV, targetedFOV, DeltaTime, 15.0f);
		SetFOV(finalFov);

		if (tpsCharacter->GetTpsCharacterStatesComponent()->IsAiming() == false)
		{
			GEngine->AddOnScreenDebugMessage(256, 0.0f, FColor::Red, FString::SanitizeFloat(DefaultFOV));// ("Not Aiming"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(256, 0.0f, FColor::Blue, FString::SanitizeFloat(DefaultFOV));// FString("Aiming"));
		}		
	}
	*/
}

void ATpsPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);
	/*
	UpdateFOV(DeltaTime);

	AGameCharacterBase* tpsCharacter = PCOwner ? Cast<AGameCharacterBase>(PCOwner->GetPawn()) : nullptr;

	if (tpsCharacter)
	{
		tpsCharacter->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
	}
	*/
}
