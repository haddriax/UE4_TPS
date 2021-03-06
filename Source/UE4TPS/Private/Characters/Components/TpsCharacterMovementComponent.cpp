// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/TpsCharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

#include "Characters/TpsCharacterBase.h"
#include "Characters/Components/CharacterWeaponComponent.h"


UTpsCharacterMovementComponent::UTpsCharacterMovementComponent()
{
	CharaMovementState.WalkConfig.ForwardSpeed = 128.195f;
	CharaMovementState.WalkConfig.BackwardSpeed = 103.015f;
	CharaMovementState.WalkConfig.SideSpeed = 102.243f;
	CharaMovementState.WalkConfig.Stance = ECharacterStance::Walk;

	CharaMovementState.JogConfig.ForwardSpeed = 346.000f;
	CharaMovementState.JogConfig.BackwardSpeed = 248.388f;
	CharaMovementState.JogConfig.SideSpeed = 310.637f;
	CharaMovementState.JogConfig.Stance = ECharacterStance::Jog;

	CharaMovementState.RunConfig.ForwardSpeed = 630.450f;
	CharaMovementState.RunConfig.BackwardSpeed = 346.000f;
	CharaMovementState.RunConfig.SideSpeed = 346.000f;
	CharaMovementState.RunConfig.Stance = ECharacterStance::Run;

	CharaMovementState.SprintRecoveryTime = 0.20f;

	ActiveConfig = &(CharaMovementState.JogConfig);

	bOrientRotationToMovement = false;
	bIgnoreBaseRotation = false;
	bUseControllerDesiredRotation = true;

	SprintLastTime = 0.0f;

	RotationRate = FRotator(0, 300, 0);
}

void UTpsCharacterMovementComponent::OnWeaponEquipped(AModularWeapon* NewlyEquippedWeapon)
{
	EnableCombatMode();
}

void UTpsCharacterMovementComponent::OnWeaponUnequipped(AModularWeapon* NewlyEquippedWeapon)
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
	RotationRate =  Velocity.SizeSquared() > 0 ? FRotator(0, 1500, 0) : FRotator(0, 300, 0);

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

	// Apply the stored input vector to X movement.
	if ((Controller && (InputDirection.X != 0.0f)))
	{
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);

		const FVector WorldDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddInputVector(WorldDirection * InputDirection.X, false);
	}

	// Apply the stored input vector to Y movement.
	if ((Controller && (InputDirection.Y != 0.0f)))
	{
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);

		const FVector WorldDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddInputVector(WorldDirection * InputDirection.Y, false);
	}

	// Stored to ensure we override the base CharacterMovement Rotation.
	// const FRotator ActorRotation = GetOwner()->GetActorRotation();
	
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

bool UTpsCharacterMovementComponent::CanRun()
{
	const UWorld* World = GetWorld();
	if (World)
	{
		return ((SprintLastTime + CharaMovementState.SprintRecoveryTime) <= (World->TimeSeconds));
	}
	
	return false;
}

ECharacterStance UTpsCharacterMovementComponent::LoadMovementConfigs(ECharacterStance NewMovementConfig)
{
	const ECharacterStance OldStance = ActiveConfig->Stance;

	switch (NewMovementConfig)
	{
	case ECharacterStance::Crouch:
		ActiveConfig = &(CharaMovementState.CrouchConfig);
		unimplemented();
		break;
	case ECharacterStance::Walk:
		ActiveConfig = &(CharaMovementState.WalkConfig);
		break;
	case ECharacterStance::Jog:
		ActiveConfig = &(CharaMovementState.JogConfig);
		break;
	case ECharacterStance::Run:
		ActiveConfig = &(CharaMovementState.RunConfig);
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

void UTpsCharacterMovementComponent::StartJump()
{
	if (TpsCharacter)
	{
		TpsCharacter->Jump();
	}
}

void UTpsCharacterMovementComponent::StopJump()
{
	if (TpsCharacter)
	{
		TpsCharacter->StopJumping();
	}
}

void UTpsCharacterMovementComponent::EndJump()
{
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
	if (CanRun())
	{
		LoadMovementConfigs(ECharacterStance::Run);

		if (OnSprintStart.IsBound())
		{
			OnSprintStart.Broadcast();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, (FString(__FUNCTION__) + FString(" : Sprint CD")));
	}
}

void UTpsCharacterMovementComponent::DisableSprint()
{	
	if (IsRunStance())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			SprintLastTime = World->TimeSeconds;
		}

		LoadMovementConfigs(ECharacterStance::Jog);
	
		if (OnSprintStop.IsBound())
		{
			OnSprintStop.Broadcast();
		}
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