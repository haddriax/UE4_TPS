// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/WeaponBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"

#include "Characters/TpsCharacterBase.h"
#include "Weapons/Components/WeaponFeedbacksComponent.h"

AWeaponBase::AWeaponBase()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	WeaponMesh->CastShadow = true;
	WeaponMesh->bReceivesDecals = false;
	WeaponMesh->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	SetRootComponent(WeaponMesh);

	WeaponFeedbacksComponent = CreateDefaultSubobject<UWeaponFeedbacksComponent>("WeaponFeedbacks");
}

void AWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AmmunitionsReserve = FMath::Clamp(WeaponDatas.AmmunitionsPerClip * WeaponDatas.InitialClips, 0, WeaponDatas.MaxAmmunitions);
	AmmunitionsInClip = 0;
	AmmunitionsInClip = GetAmmuntionsFromReserve(WeaponDatas.AmmunitionsPerClip);

	check(WeaponDatas.RateOfFire > 0);

	// Get the time between 2 shots from the rate of fire.
	TimeBetweenShots = 1 / WeaponDatas.RateOfFire;
}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (ParentCharacter)
	{
		AttachToPawnHoslterSlot(ParentCharacter);
	}
}

void AWeaponBase::AttachToPawnHand(ATpsCharacterBase* Character)
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

		// WeaponMesh->AttachToComponent(Character->GetMesh(), attachmentTransformRules, Character->GetWeaponInHandAttachPointOnCharacter());
		AttachToSocket(Character->GetMesh(), Character->GetWeaponInHandAttachPointOnCharacter());
		WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
		// WeaponMesh->SetHiddenInGame(false);
	}
}

void AWeaponBase::AttachToPawnHoslterSlot(ATpsCharacterBase* Character)
{
	bool AttachSucceed = false;

	switch (WeaponDatas.WeaponSlot)
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

bool AWeaponBase::AttachToSocket(USkeletalMeshComponent* MeshToAttachOn, FName SocketName)
{
	bool bIsSuccess = WeaponMesh->AttachToComponent(MeshToAttachOn, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	SetActorRelativeLocation(FVector::ZeroVector);
	return bIsSuccess;
}

void AWeaponBase::DetachFromPawn()
{
	// Detach from the hand socket.
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	AttachToPawnHoslterSlot(ParentCharacter);
}

void AWeaponBase::OnBurstStarted()
{
	const float gameTime = GetWorld()->GetTimeSeconds();

	WeaponFeedbacksComponent->StartSimulatingWeaponFiring();

	// If LastFireTime && WeaponDatas.TimeBetweenShots are valid, and the TimeBetweenShots is NOT elapsed.
	if (LastFireTime > 0 && GetTimeBetweenShots() > 0.0f && (LastFireTime + GetTimeBetweenShots() > gameTime))
	{
		if (WeaponDatas.bIsAutomaticWeapon)
		{
			// Set a non looping Timer which will manage firing when ready.
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring,
				this,
				&AWeaponBase::HandleRefiring,
				LastFireTime + GetTimeBetweenShots() - gameTime,
				false);
		}

	}
	else
	{
		// Directly HandleFiring.
		HandleFiring();
	}
}

void AWeaponBase::OnBurstEnded()
{
	BurstCounter = 0;

	WeaponFeedbacksComponent->StopSimulatingWeaponFiring();

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);

	bRefiring = false;

	ResetSpread();
}

void AWeaponBase::FireWeapon()
{
	// Should not be called on base class.
	unimplemented();
}

void AWeaponBase::HandleFiring()
{
	if (CanFire())
	{
		BurstCounter++;

		ConsumeAmmunition();

		WeaponFeedbacksComponent->SimulateWeaponFiring();

		FireWeapon();

		GEngine->AddOnScreenDebugMessage(64, 10.0f, FColor::Blue, FString::SanitizeFloat(GetTimeBetweenShots()));
		GEngine->AddOnScreenDebugMessage(64, 10.0f, FColor::Red, FString::SanitizeFloat(IsFiring()));

		// Handle Refiring.
		bRefiring = 
			IsFiring()
			&& GetTimeBetweenShots() > 0.0f
			&& WeaponDatas.bIsAutomaticWeapon;

		// Broadcast the OnShot Event.
		if (OnShot.IsBound())
		{
			OnShot.Broadcast();
		}

		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring,
				this,
				&AWeaponBase::HandleRefiring,
				GetTimeBetweenShots(),
				false
			);
		}
		else
		{
			SetWeaponState(EWeaponState::Idle);
		}
	}
	// Can't shoot in the middle of a burst.
	else if (BurstCounter > 0)
	{
		// End the burst if we cannot fire anymore (ex : out of ammo).
		OnBurstEnded();
	}

	// Keep the last fire time, so we know if we can shot again at a given frame.
	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AWeaponBase::HandleRefiring()
{
	HandleFiring();

	IncreaseSpread();
}

int32 AWeaponBase::AddRandomDirectionFromCone(FVector& DirectionToModify, float HorizontalAngle, float VerticalAngle, bool bAreRadiantAngles /*= true*/)
{
	const int32 RandomSeed = FMath::Rand();
	const FRandomStream rnStream(RandomSeed);

	if (bAreRadiantAngles)
	{
		DirectionToModify = rnStream.VRandCone(DirectionToModify, HorizontalAngle, VerticalAngle);
	}
	else
	{
		DirectionToModify = rnStream.VRandCone(DirectionToModify, FMath::DegreesToRadians(HorizontalAngle), FMath::DegreesToRadians(VerticalAngle));
	}

	return RandomSeed;
}

const FName AWeaponBase::GetMuzzleAttachPoint() const
{
	return MuzzleFX_SocketName;
}

void AWeaponBase::SetPlayer(ATpsCharacterBase* _Player)
{
	ParentCharacter = _Player;
}

void AWeaponBase::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PreviousState = CurrentState;
	CurrentState = NewState;

	// If we finished firing ...
	if (PreviousState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstEnded();

		if (OnFireStop.IsBound())
		{
			OnFireStop.Execute();
		}
	}

	// If we should start firing ...
	if (PreviousState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();

		if (OnFireStart.IsBound())
		{
			OnFireStart.Execute();
		}
	}
}

FVector AWeaponBase::GetMuzzleWorldLocation() const
{
	return GetMesh()->GetSocketLocation(GetMuzzleAttachPoint());
}

FVector AWeaponBase::GetShotWorldDirection() const
{
	return GetMesh()->GetSocketLocation(GetShotDirectionSocketName()) - GetMesh()->GetSocketLocation(GetMuzzleAttachPoint());
}

void AWeaponBase::TryShooting()
{
	if (!IsFiring() && CanFire())
	{
		SetWeaponState(EWeaponState::Firing);
	}
}

void AWeaponBase::StopShooting()
{
	if (IsFiring())
	{
		SetWeaponState(EWeaponState::Idle);
	}
}

bool AWeaponBase::CanFire() const
{
	// Can fire if is doing nothing or if is already firing.
	bool bStateAllowFire = (IsIdling() || IsFiring());

	// Ensure Player is referenced and not reloading.
	return ((bStateAllowFire) && (!IsPendingReload()) && (AmmunitionsInClip > 0));
}

bool AWeaponBase::CanReload() const
{
	bool bStateAllowReload = (!IsPendingEquip())
		&& (!IsPendingUnequip())
		&& (!IsPendingReload())
		&& (AmmunitionsReserve > 0)
		&& (GetAmmunitionInClip() < GetClipSize());

	return bStateAllowReload;
}

bool AWeaponBase::CanSwitchWeapon() const
{
	return IsIdling()
		|| IsFiring();
}

bool AWeaponBase::EquipOn(ATpsCharacterBase* _Character)
{
	if (CanSwitchWeapon())
	{
		SetPlayer(_Character);

		SetWeaponState(EWeaponState::Equipping);

		WeaponFeedbacksComponent->SimulateRaiseWeapon();

		return true;
	}

	return false;
}

void AWeaponBase::EquipFinished()
{
	SetWeaponState(EWeaponState::Idle);
	bIsEquipped = true;
}

bool AWeaponBase::Unequip()
{
	if (CanSwitchWeapon())
	{
		SetWeaponState(EWeaponState::Unequipping);

		WeaponFeedbacksComponent->SimulateLowerWeapon();

		return true;
	}

	return false;
}

void AWeaponBase::UnequipFinished()
{
	SetWeaponState(EWeaponState::Idle);
	bIsEquipped = false;

	SetPlayer(nullptr);
}

void AWeaponBase::Reload(float ReloadDuration)
{
	if (CanReload())
	{
		// Reloading can cancel shooting.
		StopShooting();

		SetWeaponState(EWeaponState::Reloading);

		WeaponFeedbacksComponent->SimulateReloading();

		// Prepare the ReloadFinished call.
		GetWorldTimerManager().SetTimer(
			TimerHandle_WeaponAction,
			this,
			&AWeaponBase::OnReloadFinished,
			ReloadDuration,
			false);
	}
}

void AWeaponBase::OnReloadFinished()
{
	int32 addToClip = GetAmmuntionsFromReserve(GetMissingAmmoInClip());

	AddAmmuntionsInClip(addToClip);

	SetWeaponState(EWeaponState::Idle);
}

int32 AWeaponBase::GetAmmuntionsFromReserve(int32 Amount)
{
	int32 pulledAmmo = 0;

	pulledAmmo = FMath::Min(AmmunitionsReserve, Amount);

	AmmunitionsReserve -= pulledAmmo;

	return pulledAmmo;
}

int32 AWeaponBase::GetMissingAmmoInClip() const
{
	return WeaponDatas.AmmunitionsPerClip - AmmunitionsInClip;
}

void AWeaponBase::AddAmmuntionsInClip(int32 Amount)
{
	AmmunitionsInClip = FMath::Clamp(AmmunitionsInClip + Amount, 0, WeaponDatas.AmmunitionsPerClip);
}

void AWeaponBase::ConsumeAmmunition()
{
	AmmunitionsInClip = FMath::Clamp(AmmunitionsInClip - 1, 0, WeaponDatas.AmmunitionsPerClip);
}

void AWeaponBase::IncreaseSpread()
{
	CurrentSpread = FMath::Clamp(
		CurrentSpread + (WeaponDatas.MaxSpreadAngle / WeaponDatas.ShotForMaxSpread) * BurstCounter,
		WeaponDatas.MinSpreadAngle,
		WeaponDatas.MaxSpreadAngle
	);
}

void AWeaponBase::ResetSpread()
{
	CurrentSpread = 0;
}