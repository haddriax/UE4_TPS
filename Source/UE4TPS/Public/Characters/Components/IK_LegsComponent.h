// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IK_LegsComponent.generated.h"

class ATpsCharacter;


USTRUCT()
struct FIKFootDatas
{
	GENERATED_BODY();

	UPROPERTY()
		uint8 bIsRightFoot;
	UPROPERTY()
		FVector GroundNormal;
	UPROPERTY()
		FVector TargetedLocation;
	UPROPERTY()
		FRotator TargetedRotation;
	UPROPERTY()
		float SquaredDistanceFromTargetedLocation;
	UPROPERTY()
		float Alpha;

	FIKFootDatas()
	{
		bIsRightFoot = 1;
		GroundNormal = FVector::ZeroVector;
		TargetedLocation = FVector::ZeroVector;
		TargetedRotation = FRotator::ZeroRotator;
		SquaredDistanceFromTargetedLocation = 0.0f;
		Alpha = 0.0f;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4TPS_API UIK_LegsComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/*
	* The Bone or Socket from where the Left Foot trace start.
	*/
	UPROPERTY(EditAnywhere, Category = "IK Bones")
		FName LeftFootSocketName = "foot_l";
	/*
	* The Bone or Socket from where the Right Foot trace start.
	*/
	UPROPERTY(EditAnywhere, Category = "IK Bones")
		FName RightFootSocketName = "foot_r";

	UPROPERTY(EditAnywhere, Category = "IK Bones")
		FName LeftFootIKBoneName = "ik_foot_r";

	UPROPERTY(EditAnywhere, Category = "IK Bones")
		FName RightFootIKBoneName = "ik_foot_r";

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		float RightFootOffset;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		float LeftFootOffset;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		float PelvisOffset;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		FRotator RightFootRotation;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		FRotator LeftFootRotation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float TraceRangeBelowCapsule = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float FeetsLocationInterpolationSpeed = 25.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float FeetsRotationInterpolationSpeed = 35.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float PelvisInterpolationSpeed = 15.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float CapsuleInterpolationSpeed = 10.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "IK")
		float AlphaInterpolationSpeed = 5.0f;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		float RightFootAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
		float LeftFootAlpha;

	UPROPERTY(EditAnywhere, Category = "IK")
		uint8 bIkTraceComplex = true;

	FVector RightFootPreviousLocation;
	FVector LeftFootPreviousLocation;

	/*
	* The default Capsule half height.
	*/
	float IK_CapsuleHalfHeight;


	FVector RightFootEffectorLocation;
	FVector LeftFootEffectorLocation;

	ATpsCharacter* Character = nullptr;

public:
	// Sets default values for this component's properties
	UIK_LegsComponent();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		float GetRightFootAlpha() const { return RightFootAlpha; }
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		float GetLeftFootAlpha() const { return LeftFootAlpha; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool CheckOwnerValid();
	bool CheckSocketsValid();

	bool TraceFromFootAndHydrateDatas(FIKFootDatas& IKFootDatas);

	float IKTraceFromFoot(FName const& BoneName, /* OUT */ FVector& Normal);

	void UpdateFootRotation();
	FVector ConvertOffsetToEffector(float Offset, bool bIsRightFoot);

	/*
	* Convert an Impact normal to a Rotator.
	*/
	FRotator NormalToRotator(FVector const& Normal) const;

	/*
	* Resize the Capsule to match Character Pose.
	*/
	void UpdateCapsule(float HipsShift, bool ResetToDefault, float DeltaTime);

	FRotator ComputeRotationOffset(FRotator const& CurrentRotation, FVector const& GroundNormal) const;


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
