// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4TPS/Public/Characters/Components/ThirdPersonAdvancedAnimComponent.h"

#include "Engine/World.h"

#include "GameFramework/Character.h" 	
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"

// Sets default values for this component's properties
UThirdPersonAdvancedAnimComponent::UThirdPersonAdvancedAnimComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

}

void UThirdPersonAdvancedAnimComponent::InitializeComponent()
{
	const UWorld* lWorld = GetWorld();
	if (lWorld && lWorld->IsGameWorld())
	{

	}
}


// Called when the game starts
void UThirdPersonAdvancedAnimComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());
	check(Character);

	MotionKeys.Reserve(MotionKeyNumber);

	for (int32 i = 0; i < MotionKeyNumber; ++i)
	{
		MotionKeys.Add(
			FMotionKey(
				0.0f,
				Character->GetActorLocation(),
				Character->GetVelocity(),
				Character->GetActorRotation()
			)
		);
	}

	Character->GetWorldTimerManager().SetTimer(TimerHandle_MotionKeysCapture, this, &UThirdPersonAdvancedAnimComponent::SaveMotionKey, MotionKeysCaptureRate, true, 0.0f);
}


// Called every frame
void UThirdPersonAdvancedAnimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Draw the Circle around the player.
	DrawDebugCircle(
		GetWorld(),
		Character->GetActorLocation() + Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::DownVector,
		Character->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		64,
		FColor::Emerald,
		false,
		-1.0f,
		0,
		1.0f,
		FVector::RightVector,
		FVector::ForwardVector,
		false
	);

	float const MovementDirection =
		FVector::DotProduct(
			Character->GetCharacterMovement()->GetLastInputVector().GetSafeNormal(),
			Character->GetCharacterMovement()->GetPendingInputVector().GetSafeNormal()
		);

	// GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Purple, FString::SanitizeFloat(MovementDirection));
}

float UThirdPersonAdvancedAnimComponent::GetPawnYawDelta()
{
	float const InterpSpeed = 10.0f;

	FMath::RInterpTo(Character->GetActorRotation(), LastTickPawnRotation, Character->GetWorld()->GetDeltaSeconds(), InterpSpeed);
	return 0.0f;
}

float UThirdPersonAdvancedAnimComponent::GetControllerYawDelta()
{
	return 0.0f;
}


void UThirdPersonAdvancedAnimComponent::SaveMotionKey()
{
	MotionKeys.RemoveAt(0);

	MotionKeys.Add(
		FMotionKey(
			Character->GetWorld()->GetTimeSeconds(),
			Character->GetActorLocation() + Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::DownVector,
			Character->GetVelocity(),
			Character->GetActorRotation()
		)
	);
	MotionKeys[MotionKeyNumber - 1].ExpectedLocation = PredictNextMovement();

	DrawDebugPoint
	(
		GetWorld(),
		MotionKeys[MotionKeyNumber - 1].Location + FVector::UpVector * 1,
		5.0f,
		FColor::Red,
		false,
		MotionKeysCaptureRate * MotionKeyNumber,
		0
	);

	ComputeTurnStrength();
}

void UThirdPersonAdvancedAnimComponent::DebugDrawHistory()
{

}

float UThirdPersonAdvancedAnimComponent::ComputeTurnStrength()
{
	// Compute the angle between the first two keys, giving the turn direction.
	float Angle = MotionKeys[MotionKeyNumber - 2].Location.CosineAngle2D(MotionKeys[MotionKeyNumber - 1].Location);

	// Reverse for looking form the newest Key to the oldest.
	for (int32 i = MotionKeyNumber - 1; i >= 0; --i)
	{

	}

	return 0.0f;
}

FVector UThirdPersonAdvancedAnimComponent::PredictNextMovement()
{
	TArray<FVector> TranslationBetweenLocations;
	TranslationBetweenLocations.Reserve(MotionKeyNumber - 1);

	for (int32 i = (MotionKeyNumber - 1); i >= 1; --i)
	{
		TranslationBetweenLocations.Add(MotionKeys[i].Location - MotionKeys[i - 1].Location);
	}

	FVector Loc;
	for (int32 i = 0; i < (MotionKeyNumber - 1); ++i)
	{
		Loc += TranslationBetweenLocations[i];
	}

	return Character->GetActorLocation() + (Loc / MotionKeyNumber) + Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::DownVector;
}
