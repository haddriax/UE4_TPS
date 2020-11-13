// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Components/TpsCharacterStatesComponent.h"

#include "Characters/TpsCharacterBase.h"
#include "Characters/Components/TpsCharacterMovementComponent.h"
#include "Characters/Components/WeaponHandlerComponent.h"
#include "Weapons/WeaponBase.h"

// Sets default values for this component's properties
UTpsCharacterStatesComponent::UTpsCharacterStatesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	// ...
}

void UTpsCharacterStatesComponent::SetWantsToAim(bool Value)
{
	if (CharacterState.bIsInTransition == false)
	{
		CharacterState.bWantsToAim = Value;
	}
}

void UTpsCharacterStatesComponent::SetWantsToSprint(bool Value)
{
	if (CharacterState.bIsInTransition == false)
	{
		CharacterState.bWantsToSprint = Value;
	}
}

void UTpsCharacterStatesComponent::SetWantsToFire(bool Value)
{
	if (CharacterState.bIsInTransition == false)
	{
		CharacterState.bWantsToFire = Value;
	}
}

void UTpsCharacterStatesComponent::TryJump(bool Value)
{
}

void UTpsCharacterStatesComponent::TryReload(bool Value)
{
}

bool UTpsCharacterStatesComponent::CanAimInCurentState() const
{
	// Can't Aim if Sprinting.
	return CharacterState.bIsSprinting == false;
}

bool UTpsCharacterStatesComponent::CanFireInCurentState() const
{
	return Character->GetWeaponHandlerComponent()->GetEquippedWeapon()->CanFire()
		&& CharacterState.bIsSprinting == false;
}

bool UTpsCharacterStatesComponent::CanSprintInCurentState() const
{
	return CharacterState.bIsInTransition == false;
}

// Called when the game starts
void UTpsCharacterStatesComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ATpsCharacterBase>(GetOwner());

	check(Character);

	// DefaultSpeed = Character->GetTpsCharacterMovementComponent()->MaxWalkSpeed;
}

void UTpsCharacterStatesComponent::EnterSprint()
{
	// Character->GetTpsCharacterMovementComponent()->MaxWalkSpeed = (DefaultSpeed * SprintSpeedMultiplier);

	// Cancel fire and aim attempt.
	CharacterState.bWantsToCrouch = false;
	CharacterState.bWantsToFire = false;
	CharacterState.bWantsToAim = false;
	CharacterState.bWantsToStand = false;

	// Cancel fire and aim stances.
	CharacterState.bIsFiring = false;
	CharacterState.bIsAiming = false;
	CharacterState.bIsStanding = false;

	// Apply.
	CharacterState.bIsSprinting = true;

	// GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Transition, )

	// Notify player.
	// Character->OnStartSprint();
}

void UTpsCharacterStatesComponent::LeaveSprint()
{
	CharacterState.bWantsToSprint = false;
	CharacterState.bIsSprinting = false;
}


void UTpsCharacterStatesComponent::UpdateState()
{
	// If a transition is already occurring, then wait for it to finish i.e. do nothing this update.
	if (CharacterState.bIsInTransition == true)
	{
		return;
	}

	check(Character->GetTpsCharacterMovementComponent());

	// Check SPRINT stance.
	if (WantsToSprint() == true)
	{
		FVector velocity = Character->GetTpsCharacterMovementComponent()->Velocity;

		if (CanSprintInCurentState() == true)
		{

			EnterSprint();
			return;
		}
	}
	else if (CharacterState.bIsSprinting)
	{
		LeaveSprint();
	}

	if (WantsToFire() == true)
	{
		if (CanFireInCurentState() == true)
		{
			// Cancel Sprint attempt.
			CharacterState.bWantsToSprint = false;

			// Cancel Sprint stances.
			CharacterState.bIsSprinting = false;

			// Apply.
			CharacterState.bIsFiring = true;

			// Notify player.
			Character->GetWeaponHandlerComponent()->GetEquippedWeapon()->StartFireOrder();

			// Subscribe to the one shot event OnFireStop of the weapon.
			Character->GetWeaponHandlerComponent()->GetEquippedWeapon()->OnFireStop.BindUObject(this, &UTpsCharacterStatesComponent::NotifyStopFiring);
		}
	}
	else if (CharacterState.bIsFiring == true)
	{
		Character->GetWeaponHandlerComponent()->GetEquippedWeapon()->StopFireOrder();
		CharacterState.bWantsToFire = false;
		CharacterState.bIsFiring = false;
	}

	// Check AIM stance.
	if (WantsToAim() == true)
	{
		if (CanAimInCurentState() == true)
		{
			// Character->GetTpsCharacterMovementComponent()->MaxWalkSpeed = (DefaultSpeed * ScopeSpeedMultiplier);

			// Cancel Sprint attempt.
			CharacterState.bWantsToSprint = false;

			// Cancel Sprint stance.
			CharacterState.bIsSprinting = false;

			// Apply.
			CharacterState.bIsAiming = true;

			// Notify player.
			// ...
		}
	}
	else if (CharacterState.bIsAiming)
	{
		CharacterState.bIsAiming = false;
	}

	CharacterState.bWantsToStand = true;
	CharacterState.bIsStanding = true;
}

void UTpsCharacterStatesComponent::StartTransitionForDuration(float Duration)
{
	GetOwner()->GetWorldTimerManager().SetTimer(
		TimerHandle_Transition,
		this,
		&UTpsCharacterStatesComponent::TransitionEnd,
		Duration,
		false);
}

void UTpsCharacterStatesComponent::TransitionEnd()
{
	CharacterState.bIsInTransition = false;
}

void UTpsCharacterStatesComponent::NotifyStopFiring()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString("Stopped firing"));
}

void UTpsCharacterStatesComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}

void UTpsCharacterStatesComponent::InitializeComponent()
{
	Super::InitializeComponent();

	check(Character);
}

// Called every frame
void UTpsCharacterStatesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateState();
}

void UTpsCharacterStatesComponent::TryAiming()
{
	SetWantsToAim(true);
}

void UTpsCharacterStatesComponent::StopTryingAiming()
{
	SetWantsToAim(false);
}
