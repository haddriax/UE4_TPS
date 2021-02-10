// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/ModularWeapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"

#include "Characters/TpsCharacterBase.h"

#include "Weapons/FireMode/WeaponFireModeComponent.h"

AModularWeapon::AModularWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	WeaponMesh->CastShadow = true;
	WeaponMesh->bReceivesDecals = false;
	WeaponMesh->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	SetRootComponent(WeaponMesh);

	WeaponRightHandSocketName = "socket_hand_right";
	WeaponLeftHandSocketName = "socket_hand_left";
	MuzzleFX_SocketName = "MuzzleSocket";
	MuzzleTrace_SocketName = "MuzzleSocket";
	MuzzleDirection_SocketName = "MuzzleDirectionSocket";

	// WeaponFeedbacksComponent = CreateDefaultSubobject<UWeaponFeedbacksComponent>("WeaponFeedbacks");	
}

void AModularWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
		
	for (UActorComponent* nonCasted_fireMode : GetComponents())
	{
		UWeaponFireModeComponent* fireMode = Cast<UWeaponFireModeComponent>(nonCasted_fireMode);

		if (fireMode)
		{
			FireModes.Push(fireMode);
		}
	}
}

void AModularWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AModularWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (ParentCharacter)
	{
		AttachToPawnHoslterSlot(ParentCharacter);
	}
}

void AModularWeapon::AttachToPawnHand(ATpsCharacterBase* Character)
{
	// Attach the weapon to the player socket.
	if (Character)
	{
		FAttachmentTransformRules attachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			false
		);

		AttachToSocket(Character->GetMesh(), Character->GetWeaponInHandAttachPointOnCharacter());
		WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	}
}

void AModularWeapon::AttachToPawnHoslterSlot(ATpsCharacterBase* Character)
{
	bool AttachSucceed = false;

	switch (WeaponSlot)
	{
	case EWeaponSlot::Rifle:
		AttachSucceed = AttachToSocket(Character->GetMesh(), Character->GetHolsterWeaponAttachPointOnCharacter());

		// AttachSucceed = WeaponMesh->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Character->GetHolsterWeaponAttachPointOnCharacter());
		break;
	case EWeaponSlot::Handgun:
		unimplemented();
		break;
	default:
		WeaponMesh->SetHiddenInGame(true);
		UE_LOG(LogTemp, Error, TEXT("X %s - No matching WeaponSlot for holstering. Check if %s have it's WeaponSlot defined."), TEXT(__FUNCTION__), *GetName());
	}

	if (!AttachSucceed)
	{
		UE_LOG(LogTemp, Error, TEXT("X %s - Attaching weapon to it's holster socket failed."), TEXT(__FUNCTION__));
	}

	WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	WeaponMesh->SetHiddenInGame(false);
}

bool AModularWeapon::AttachToSocket(USkeletalMeshComponent* MeshToAttachOn, FName SocketName)
{
	bool bIsSuccess = WeaponMesh->AttachToComponent(MeshToAttachOn, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	SetActorRelativeLocation(FVector::ZeroVector);
	return bIsSuccess;
}

void AModularWeapon::DetachFromPawn()
{
	// Detach from the hand socket.
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	AttachToPawnHoslterSlot(ParentCharacter);
}

const FName AModularWeapon::GetMuzzleAttachPoint() const
{
	return MuzzleFX_SocketName;
}

void AModularWeapon::SetPlayer(ATpsCharacterBase* _Player)
{
	ParentCharacter = _Player;
}

void AModularWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PreviousState = CurrentState;
	CurrentState = NewState;

	// If we finished firing ...
	if (PreviousState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		if (FireModes.IsValidIndex(ActiveFireMode))
		{
			FireModes[ActiveFireMode]->EndFire();
		}		
	}

	// If we should start firing ...
	if (PreviousState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		if (FireModes.IsValidIndex(ActiveFireMode))
		{
			FireModes[ActiveFireMode]->BeginFire();
		}
	}
}

FVector AModularWeapon::GetMuzzleWorldLocation() const
{
	return GetMesh()->GetSocketLocation(GetMuzzleAttachPoint());
}

FVector AModularWeapon::GetShotWorldDirection() const
{
	return (GetMesh()->GetSocketLocation(GetShotDirectionSocketName()) - (GetMesh()->GetSocketLocation(GetMuzzleAttachPoint())));
}

void AModularWeapon::TryShooting()
{
	if (!IsFiring() && CanFire())
	{
		SetWeaponState(EWeaponState::Firing);
	}
}

void AModularWeapon::StopShooting()
{
	if (IsFiring())
	{
		SetWeaponState(EWeaponState::Idle);
	}
}

bool AModularWeapon::CanFire() const
{
	// Can fire if is doing nothing or if is already firing.
	bool bStateAllowFire = (IsIdling() || IsFiring());

	// Ensure Player is referenced and not reloading.
	return ((bStateAllowFire) && (!IsPendingReload()));
}

bool AModularWeapon::CanReload() const
{
	bool bStateAllowReload = (!IsPendingEquip())
		&& (!IsPendingUnequip())
		&& (!IsPendingReload());

	return bStateAllowReload;
}

bool AModularWeapon::CanSwitchWeapon() const
{
	return IsIdling()
		|| IsFiring();
}

bool AModularWeapon::EquipOn(ATpsCharacterBase* _Character)
{
	if (CanSwitchWeapon())
	{
		SetPlayer(_Character);

		SetWeaponState(EWeaponState::Equipping);

		return true;
	}

	return false;
}

void AModularWeapon::EquipFinished()
{
	SetWeaponState(EWeaponState::Idle);
	bIsEquipped = true;
}

bool AModularWeapon::Unequip()
{
	if (CanSwitchWeapon())
	{
		SetWeaponState(EWeaponState::Unequipping);

		return true;
	}

	return false;
}

void AModularWeapon::UnequipFinished()
{
	SetWeaponState(EWeaponState::Idle);
	bIsEquipped = false;

	SetPlayer(nullptr);
}

void AModularWeapon::Reload(float ReloadDuration)
{
	if (CanReload())
	{
		// Reloading can cancel shooting.
		StopShooting();

		SetWeaponState(EWeaponState::Reloading);

		// Prepare the ReloadFinished call.
		GetWorldTimerManager().SetTimer(
			TimerHandle_WeaponAction,
			this,
			&AModularWeapon::OnReloadFinished,
			ReloadDuration,
			false);
	}
}

void AModularWeapon::OnReloadFinished()
{
	// const int32 addToClip = GetAmmuntionsFromReserve(GetMissingAmmoInClip());

	// AddAmmuntionsInClip(addToClip);

	SetWeaponState(EWeaponState::Idle);
}
