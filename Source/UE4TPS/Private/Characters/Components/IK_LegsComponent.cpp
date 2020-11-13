// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4TPS/Public/Characters/Components/IK_LegsComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

#include "Characters/TpsCharacterBase.h"

// Sets default values for this component's properties
UIK_LegsComponent::UIK_LegsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UIK_LegsComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckOwnerValid();
	CheckSocketsValid();

	IK_CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	RightFootPreviousLocation = Character->GetMesh()->GetBoneLocation(RightFootSocketName, EBoneSpaces::ComponentSpace);
	LeftFootPreviousLocation = Character->GetMesh()->GetBoneLocation(LeftFootSocketName, EBoneSpaces::ComponentSpace);

	if (bIkTraceComplex == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("<!> - %s : IK will trace for Complexe geometry, disable to improve performance."), TEXT(__FUNCTION__));
	}
}

bool UIK_LegsComponent::CheckOwnerValid()
{
	Character = Cast<ATpsCharacterBase>(GetOwner());

	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Error, TEXT("X - %s : Owner is invalid. It should be an ATpsCharacter"), TEXT(__FUNCTION__));
		return false;
	}
	return true;
}

bool UIK_LegsComponent::CheckSocketsValid()
{
	const USkeletalMeshComponent*  UseMesh = Character->GetMesh();

	bool Valid = UseMesh->DoesSocketExist(LeftFootSocketName)
		&& UseMesh->DoesSocketExist(RightFootSocketName)
		&& UseMesh->DoesSocketExist(LeftFootIKBoneName)
		&& UseMesh->DoesSocketExist(RightFootIKBoneName);

	if (!Valid)
	{
		UE_LOG(LogTemp, Error, TEXT("X - %s : One or more socket are missing for IK."), TEXT(__FUNCTION__));
		return false;
	}
	return true;
}

bool UIK_LegsComponent::TraceFromFootAndHydrateDatas(FIKFootDatas& IKFootDatas)
{
	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();

	const FVector CharacterWorldLocation = Character->GetActorLocation();
	const FVector BoneWorldLocation = CharacterMesh->GetBoneLocation(
		IKFootDatas.bIsRightFoot ? RightFootSocketName : LeftFootSocketName,
		EBoneSpaces::WorldSpace
	);

	FVector TraceStart(BoneWorldLocation.X, BoneWorldLocation.Y, CharacterWorldLocation.Z);
	TraceStart = Character->GetMesh()->GetComponentTransform().InverseTransformPosition(TraceStart);

	const float EndZ = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + TraceRangeBelowCapsule;
	const FVector TraceEnd(BoneWorldLocation.X, BoneWorldLocation.Y, (CharacterWorldLocation.Z - EndZ));

	FHitResult Hit;

	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(Character->GetUniqueID());
	CQP.bTraceComplex = (bIkTraceComplex == 1);
	
	bool bTraceSucceed = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, CQP);

	if (Hit.bBlockingHit)
	{
		IKFootDatas.GroundNormal = Hit.Normal;
		IKFootDatas.SquaredDistanceFromTargetedLocation = FVector::DistSquared(TraceStart, Hit.ImpactPoint);
		// return (TraceStart - Hit.ImpactPoint).Size() - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	else
	{
		IKFootDatas.GroundNormal = FVector::ZeroVector;
	}

	return bTraceSucceed;
}

float UIK_LegsComponent::IKTraceFromFoot(FName const& BoneName, /* OUT */ FVector& OutNormal)
{
	const USkeletalMeshComponent* UseMesh = Character->GetMesh();

	const FVector CharacterWorldLocation = Character->GetActorLocation();
	const FVector BoneWorldLocation = UseMesh->GetBoneLocation(BoneName, EBoneSpaces::WorldSpace);

	const FVector TraceStart(BoneWorldLocation.X, BoneWorldLocation.Y, CharacterWorldLocation.Z);

	const float EndZ = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + TraceRangeBelowCapsule;

	const FVector TraceEnd(BoneWorldLocation.X, BoneWorldLocation.Y, (CharacterWorldLocation.Z - EndZ));

	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(Character->GetUniqueID());
	CQP.bTraceComplex = true;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, CQP);

	if (Hit.bBlockingHit)
	{
		OutNormal = Hit.Normal;
		return (TraceStart - Hit.ImpactPoint).Size() - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	else
	{
		OutNormal = FVector::ZeroVector;
		return 0.0f;
	}
}

void UIK_LegsComponent::UpdateFootRotation()
{
}

FVector UIK_LegsComponent::ConvertOffsetToEffector(float Offset, bool bIsRightFoot)
{
	return FVector(bIsRightFoot ? -Offset : Offset, 0.0f, 0.0f);
}

FRotator UIK_LegsComponent::NormalToRotator(FVector const& Normal) const
{
	float Roll = FMath::Atan2(Normal.Y, Normal.Z);
	float Pitch = FMath::Atan2(Normal.X, Normal.Z) * (-1);

	return FRotator(Pitch, 0.0f, Roll);
}

void UIK_LegsComponent::UpdateCapsule(float HipsShift, bool ResetToDefault, float DeltaTime)
{
	HipsShift = FMath::Abs(HipsShift) / 2;

	float CapsuleHalfHeight;

	if (ResetToDefault)
	{
		CapsuleHalfHeight = IK_CapsuleHalfHeight;
	}
	else
	{
		CapsuleHalfHeight = IK_CapsuleHalfHeight - HipsShift;
	}

	CapsuleHalfHeight = FMath::FInterpTo(
		Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		CapsuleHalfHeight,
		DeltaTime,
		CapsuleInterpolationSpeed
	);

	Character->GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeight, true);
}

FRotator UIK_LegsComponent::ComputeRotationOffset(FRotator const& CurrentRotation, FVector const& GroundNormal) const
{
	if (GroundNormal == FVector::ZeroVector)
		return FRotator::ZeroRotator;

	// Compute the "Right" vector of this rotation.	
	const FVector YVector = FVector::CrossProduct(GroundNormal, Character->GetActorForwardVector());

	const FQuat FootQuat = FRotationMatrix::MakeFromXZ(-YVector, GroundNormal).ToQuat();

	// Transform the Rotation from World space to Mesh space so it can be used in the AnimBP.
	const FRotator FootRotator = Character->GetMesh()->GetComponentTransform().InverseTransformRotation(FootQuat).Rotator();

	return FootRotator;
}

// Called every frame
void UIK_LegsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Normal of the ground, used as an OUT parameter.
	FVector Normal;

	// Reduce the sensitivy for the bone displacement check, avoiding any change to the Aplha when not moving.
	float constexpr ErrorMargin = 0.1f;

	// Read the current location of both of the Feets Sockets, and keep them in memory.
	FVector RightFootCurrentLocation = Character->GetMesh()->GetBoneLocation(RightFootSocketName, EBoneSpaces::ComponentSpace);
	FVector LeftFootCurrentLocation = Character->GetMesh()->GetBoneLocation(LeftFootSocketName, EBoneSpaces::ComponentSpace);

	// Interp current offset closer to targeted offset (given by LineTrace)
	RightFootOffset = FMath::FInterpTo(RightFootOffset, IKTraceFromFoot(RightFootSocketName, /* OUT */ Normal), DeltaTime, FeetsLocationInterpolationSpeed);
	RightFootRotation = FMath::RInterpTo(RightFootRotation, ComputeRotationOffset(RightFootRotation, Normal), DeltaTime, FeetsRotationInterpolationSpeed);
	

	/*
	* The Alpha is used to intensify IK when the feet is moving toward the ground.
	* It improve a lot the behavior when climbing stairs especially.
	*/

	// Interp the alpha for this foot to 0 if it is moving upward.
	if (RightFootCurrentLocation.Z > (RightFootPreviousLocation.Z + ErrorMargin))
	{
		RightFootAlpha = FMath::FInterpTo(RightFootAlpha, 0.0f, DeltaTime, AlphaInterpolationSpeed);
	}
	else
	{
		RightFootAlpha = FMath::FInterpTo(RightFootAlpha, 1.0f, DeltaTime, AlphaInterpolationSpeed);
	}
	// GEngine->AddOnScreenDebugMessage(1024, 5.0f, FColor::Blue, FString("RightFootAlpha : ") + FString::SanitizeFloat(RightFootAlpha));

	LeftFootOffset = FMath::FInterpTo(LeftFootOffset, IKTraceFromFoot(LeftFootSocketName, /* OUT */ Normal), DeltaTime, FeetsLocationInterpolationSpeed);
	LeftFootRotation = FMath::RInterpTo(LeftFootRotation, ComputeRotationOffset(LeftFootRotation, Normal), DeltaTime, FeetsRotationInterpolationSpeed);

	// Interp the alpha for this foot to 0 if it is moving upward.
	if (LeftFootCurrentLocation.Z > (LeftFootPreviousLocation.Z + ErrorMargin))
	{
		LeftFootAlpha = FMath::FInterpTo(LeftFootAlpha, 0.0f, DeltaTime, AlphaInterpolationSpeed);
	}
	else
	{
		LeftFootAlpha = FMath::FInterpTo(LeftFootAlpha, 1.0f, DeltaTime, AlphaInterpolationSpeed);
	}
	// GEngine->AddOnScreenDebugMessage(1025, 5.0f, FColor::Blue, FString("LeftFootAlpha : ") + FString::SanitizeFloat(LeftFootAlpha));

	// 
	float const TargetedPelvisOffset = FMath::Max(RightFootOffset, LeftFootOffset);
	PelvisOffset = FMath::FInterpTo(PelvisOffset, TargetedPelvisOffset, DeltaTime, PelvisInterpolationSpeed);


	// Memorize the bones positions.
	RightFootPreviousLocation = RightFootCurrentLocation;
	LeftFootPreviousLocation = LeftFootCurrentLocation;
}