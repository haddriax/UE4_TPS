// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/TpsCharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

#include "Characters/TpsCharacterBase.h"
#include "Characters/Components/WeaponHandlerComponent.h"


UTpsCharacterMovementComponent::UTpsCharacterMovementComponent()
{
	WalkConfig.ForwardSpeed = 128.195f;
	WalkConfig.BackwardSpeed = 103.015f;
	WalkConfig.SideSpeed = 102.243f;
	WalkConfig.Stance = ECharacterStance::Walk;

	JogConfig.ForwardSpeed = 346.000f;
	JogConfig.BackwardSpeed = 248.388f;
	JogConfig.SideSpeed = 310.637f;
	JogConfig.Stance = ECharacterStance::Jog;

	RunConfig.ForwardSpeed = 630.450f;
	RunConfig.BackwardSpeed = 346.000f;
	RunConfig.SideSpeed = 346.000f;
	RunConfig.Stance = ECharacterStance::Run;

	ActiveConfig = &JogConfig;

	bOrientRotationToMovement = false;
	bIgnoreBaseRotation = false;
	bUseControllerDesiredRotation = true;

	RotationRate = FRotator(0, 300, 0);
}

void UTpsCharacterMovementComponent::OnWeaponEquipped(AWeaponBase* NewlyEquippedWeapon)
{
	EnableCombatMode();
}

void UTpsCharacterMovementComponent::OnWeaponUnequipped(AWeaponBase* NewlyEquippedWeapon)
{
	EnableTravelMode();
}

void UTpsCharacterMovementComponent::SubscribeToWeaponHandlerComponent()
{
	check(TpsCharacter);
	check(TpsCharacter->GetWeaponHandlerComponent());

	// TpsCharacter->GetWeaponHandlerComponent()->OnEquipWeapon.AddUniqueDynamic(this, &UTpsCharacterMovementComponent::OnWeaponEquipped);
	// TpsCharacter->GetWeaponHandlerComponent()->OnUnequipWeapon.AddUniqueDynamic(this, &UTpsCharacterMovementComponent::OnWeaponUnequipped);
}

void UTpsCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	TpsCharacter = Cast<ATpsCharacterBase>(GetCharacterOwner());
	SubscribeToWeaponHandlerComponent();
	LoadMovementConfigs(ECharacterStance::Jog);
	EnableTravelMode();
}

void UTpsCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FRotator r = FQuat::FindBetweenNormals(CharacterOwner->GetActorForwardVector(), Velocity.GetSafeNormal()).Rotator();

	FrameYawDelta = GetLastUpdateRotation().Yaw - GetOwner()->GetActorRotation().Yaw;
	
	float UnsignedYawDelta = FMath::Abs(r.Yaw);

	// Speed with direction modifier applied; i.e slower for side movement, even slower backward.
	float DirectionnalSpeed;

	// Forward to side.
	if (UnsignedYawDelta >= 0 && UnsignedYawDelta <= 90)
	{
		DirectionnalSpeed = FMath::Lerp(ActiveConfig->ForwardSpeed, ActiveConfig->SideSpeed, UnsignedYawDelta / 90);
	}
	// Side to backward.
	else
	{
		DirectionnalSpeed = FMath::Lerp(ActiveConfig->SideSpeed, ActiveConfig->BackwardSpeed, (UnsignedYawDelta - 90) / 90);
	}

	// Apply the speed modifier.
	MaxWalkSpeed = DirectionnalSpeed;

	const AController* Controller = CharacterOwner->GetController();
	const FRotator ControlRotation = Controller->GetControlRotation();

	// Apply the stored input vector to movement.
	if ((Controller && (InputDirection.X != 0.0f)))
	{
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);

		const FVector WorldDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddInputVector(WorldDirection * InputDirection.X, false);
	}

	if ((Controller && (InputDirection.Y != 0.0f)))
	{
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);

		const FVector WorldDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddInputVector(WorldDirection * InputDirection.Y, false);
	}

	// Stored to ensure we override the base CharacterMovement Rotation.
	const FRotator ActorRotation = GetOwner()->GetActorRotation();

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*
	if (!FreeMove)
	{
		const float RotationSpeed = 10.0f;
		// FRotator NewActorRotation = FMath::RInterpTo(ActorRotation, ControlRotation, DeltaTime, RotationSpeed);
		FRotator NewActorRotation = ControlRotation;
		NewActorRotation.Pitch = ActorRotation.Pitch;
		NewActorRotation.Roll = ActorRotation.Roll;
		GetOwner()->SetActorRotation(NewActorRotation);
	}
	*/
}

void UTpsCharacterMovementComponent::MoveForward(float InputValue)
{
	InputDirection.X = InputValue;
}

void UTpsCharacterMovementComponent::MoveRight(float InputValue)
{
	InputDirection.Y = InputValue;
}

ECharacterStance UTpsCharacterMovementComponent::LoadMovementConfigs(ECharacterStance NewMovementConfig)
{
	const ECharacterStance OldStance = ActiveConfig->Stance;

	switch (NewMovementConfig)
	{
	case ECharacterStance::Crouch:
		ActiveConfig = &CrouchConfig;
		unimplemented();
		break;
	case ECharacterStance::Walk:
		ActiveConfig = &WalkConfig;
		break;
	case ECharacterStance::Jog:
		ActiveConfig = &JogConfig;
		break;
	case ECharacterStance::Run:
		ActiveConfig = &RunConfig;
		break;
	case ECharacterStance::None:
		return ECharacterStance::None;
		break;
	default:
		unimplemented();
		return ECharacterStance::None;
		break;
	}

	MaxWalkSpeed = ActiveConfig->ForwardSpeed;

	return OldStance;
}

void UTpsCharacterMovementComponent::EnableTravelMode()
{
	MovementType = ECharacterGlobalMovementMode::Travel;	
}

void UTpsCharacterMovementComponent::EnableCombatMode()
{
	MovementType = ECharacterGlobalMovementMode::Combat;
}

void UTpsCharacterMovementComponent::EnableSprint()
{
	LoadMovementConfigs(ECharacterStance::Run);

	if (OnSprintStart.IsBound())
	{
		OnSprintStart.Broadcast();
	}
}

void UTpsCharacterMovementComponent::DisableSprint()
{
	LoadMovementConfigs(ECharacterStance::Jog);

	if (OnSprintStop.IsBound())
	{
		OnSprintStop.Broadcast();
	}
}

void UTpsCharacterMovementComponent::EnableWalk()
{
	LoadMovementConfigs(ECharacterStance::Walk);
}

void UTpsCharacterMovementComponent::DisableWalk()
{
	LoadMovementConfigs(ECharacterStance::Jog);
}