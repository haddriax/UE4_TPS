// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThirdPersonAdvancedAnimComponent.generated.h"

USTRUCT()
struct FMotionKey
{
	GENERATED_BODY();

	UPROPERTY(VisibleAnywhere)
		float SaveGameTime;

	UPROPERTY(VisibleAnywhere)
		FVector Location;

	UPROPERTY(VisibleAnywhere)
		FVector Velocity;

	UPROPERTY(VisibleAnywhere)
		FRotator Rotation;

	UPROPERTY(VisibleAnywhere)
		FVector ExpectedLocation;


	FMotionKey(float _SaveGameTime, FVector _Vector, FVector _Velocity, FRotator _Rotation)
	{
		SaveGameTime = _SaveGameTime;
		Location = _Vector;
		Velocity = _Velocity;
		Rotation = _Rotation;
	}

	FMotionKey()
	{

	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4TPS_API UThirdPersonAdvancedAnimComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	class ACharacter* Character;

	UPROPERTY(VisibleAnywhere, Category = "Motion Keys")
		TArray<FMotionKey> MotionKeys;

	/*
	* Rate at which we save the location/rotation/velocity into the history. 0.03f for 30Hz capture rate.
	*/
	float MotionKeysCaptureRate = 0.03f;

	/*
	* Amount of motion keys stored.
	*/
	int32 MotionKeyNumber = 30;

	FTimerHandle TimerHandle_MotionKeysCapture;


	FRotator LastTickPawnRotation;
	FRotator LastTickControllerRotation;

public:
	// Sets default values for this component's properties
	UThirdPersonAdvancedAnimComponent();

protected:
	virtual void InitializeComponent() override;

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	float GetPawnYawDelta();

	float GetControllerYawDelta();

protected:
	UFUNCTION()
		void SaveMotionKey();

	/*
	* Use the stored motion key to predict the next one.
	*/
	FMotionKey ApproxFuturMotionKey() const;

	void DebugDrawHistory();

	/*
	* Compute the avg from all the motion key from the turn movement.
	*/
	float ComputeTurnStrength();

	FVector PredictNextMovement();
};
