// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/TpsCharacterMovementComponent.h"

#include "Characters/TpsCharacterBase.h"
#include "Characters/Components/WeaponHandlerComponent.h"


UTpsCharacterMovementComponent::UTpsCharacterMovementComponent()
{
	WalkConfig.ForwardSpeed = 128.195f;
	JogConfig.ForwardSpeed = 346.000f;
	RunConfig.ForwardSpeed = 630.450f;
}

/*
void UTpsCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{

	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::TESTMOVE_Sprinting:
		PhysCustomSprinting(deltaTime, Iterations);
		break;
	case (uint8)ECustomMovementMode::TESTMOVE_WalkAiming:
		PhysCustomAiming(deltaTime, Iterations);
		break;
	default:
		break;
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UTpsCharacterMovementComponent::PhysCustomSprinting(float deltaTime, int32 Iterations)
{
	GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Green, FString(TEXT(__FUNCTION__)));
}

void UTpsCharacterMovementComponent::PhysCustomAiming(float deltaTime, int32 Iterations)
{
	GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Yellow, FString(TEXT(__FUNCTION__)));
}

void UTpsCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (CanAimInCurrentState() && WantToAim())
	{
		SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::TESTMOVE_WalkAiming);
	}
	else
	{
		CustomMovementMode = (uint8)ECustomMovementMode::TESTMOVE_NONE;
		SetMovementMode(MOVE_Walking);
	}

}

void UTpsCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (WantsToAim() && CanAimInCurrentState())
	{
		MaxWalkSpeed = 200;
		MaxWalkSpeedCrouched = 100;
	}
	else
	{
		MaxWalkSpeed = 600;
		MaxWalkSpeedCrouched = 300;
	}

}

void UTpsCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);


	if (CustomMovementMode == (uint8)ECustomMovementMode::TESTMOVE_WalkAiming
	&& PreviousCustomMode != (uint8)ECustomMovementMode::TESTMOVE_WalkAiming)
	{
		MaxWalkSpeed = 100;
	}
}
*/

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

void UTpsCharacterMovementComponent::ApplyMovementConfigs(FMovementConfig const& MovementConfigs)
{
	ForwardMovementResponsivity = MovementConfigs.ForwardMovementResponsivity;
	RightMovementResponsivity = MovementConfigs.ForwardMovementResponsivity;
	MaxWalkSpeed = MovementConfigs.ForwardSpeed;
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
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTpsCharacterMovementComponent::MoveForward(float InputValue)
{
	AController const* const Controller = CharacterOwner->GetController();
	if ((Controller && (InputValue != 0.0f)))
	{
		// Find out which way is forward.
		FRotator const Rotation = Controller->GetControlRotation();
		FRotator const YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector.
		FVector const WorldDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddInputVector(WorldDirection * InputValue * GetForwardMovementResponsivity(), false);
	}
}

void UTpsCharacterMovementComponent::MoveRight(float InputValue)
{
	AController const* const Controller = CharacterOwner->GetController();
	if ((Controller && (InputValue != 0.0f)))
	{
		// Find out which way is right.
		FRotator const Rotation = Controller->GetControlRotation();
		FRotator const YawRotation(0, Rotation.Yaw, 0);

		// Get right vector.
		FVector const WorldDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddInputVector(WorldDirection * InputValue * GetRightMovementResponsivity(), false);
	}
}

ECharacterStance UTpsCharacterMovementComponent::LoadMovementConfigs(ECharacterStance NewMovementConfig)
{
	ECharacterStance const OldStance = LoadedCharacterStance;

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

	LoadedCharacterStance = NewMovementConfig;

	MaxWalkSpeed = ActiveConfig->ForwardSpeed;
	ForwardMovementResponsivity = ActiveConfig->ForwardMovementResponsivity;
	RightMovementResponsivity = ActiveConfig->RightMovementResponsivity;

	return OldStance;
}

void UTpsCharacterMovementComponent::EnableTravelMode()
{
	MovementType = ECharacterGlobalMovementMode::Travel;

	bOrientRotationToMovement = true;
	TpsCharacter->bUseControllerRotationYaw = false;
}

void UTpsCharacterMovementComponent::EnableCombatMode()
{
	MovementType = ECharacterGlobalMovementMode::Combat;

	bOrientRotationToMovement = false;
	TpsCharacter->bUseControllerRotationYaw = true;
}

void UTpsCharacterMovementComponent::EnableSprint()
{
	LoadMovementConfigs(ECharacterStance::Run);
	LoadedCharacterStance = ECharacterStance::Run;

	ForwardMovementResponsivity = SprintForwardMovementResponsivity;
	RightMovementResponsivity = SprintRightMovementResponsivity;

	if (OnSprintStart.IsBound())
	{
		OnSprintStart.Broadcast();
	}
}

void UTpsCharacterMovementComponent::DisableSprint()
{
	LoadMovementConfigs(ECharacterStance::Jog);
	LoadedCharacterStance = ECharacterStance::Jog;

	if (OnSprintStop.IsBound())
	{
		OnSprintStop.Broadcast();
	}
}

void UTpsCharacterMovementComponent::EnableWalk()
{
	LoadMovementConfigs(ECharacterStance::Walk);
	LoadedCharacterStance = ECharacterStance::Walk;
}

void UTpsCharacterMovementComponent::DisableWalk()
{
	LoadMovementConfigs(ECharacterStance::Jog);
	LoadedCharacterStance = ECharacterStance::Walk;
}