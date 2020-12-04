// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/WeaponHandlerComponent.h"

#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
// #include "Engine/StreamableManager.h"

#include "Characters/Controllers/TpsPlayerController.h"
#include "Characters/TpsCharacterBase.h"
#include "Weapons/WeaponBase.h"

// Sets default values for this component's properties
UWeaponHandlerComponent::UWeaponHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// ...
}

// Called every frame
void UWeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	DrawWeaponSight();
}

void UWeaponHandlerComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	SetState(EWeaponHoldingState::NoWeapon);
}

bool UWeaponHandlerComponent::IsEquippingAnyWeapon() const
{
	return (WeaponHoldingState == EWeaponHoldingState::EquippingPrimary);
}

bool UWeaponHandlerComponent::IsUnequippingAnyWeapon() const
{
	return (WeaponHoldingState == EWeaponHoldingState::UnequippingPrimary);
}

bool UWeaponHandlerComponent::IsAnyWeaponEquipped() const
{
	return (EquippedWeapon != nullptr);
}

bool UWeaponHandlerComponent::IsPrimaryEquipped() const
{
	return (WeaponHoldingState == EWeaponHoldingState::PrimaryIdle);
}

bool UWeaponHandlerComponent::IsPendingReload() const
{
	return (WeaponHoldingState == EWeaponHoldingState::ReloadingPrimary);
}

bool UWeaponHandlerComponent::CanSwitchWeapon() const
{
	return (!IsEquippingAnyWeapon())
		&& (!IsUnequippingAnyWeapon())
		&& (!IsPendingReload());
}

bool UWeaponHandlerComponent::CanReload() const
{
	if (!GetEquippedWeapon())
		return false;
	else
		return (!IsPendingReload())
		&& (!IsEquippingAnyWeapon())
		&& (!IsUnequippingAnyWeapon())
		&& (GetEquippedWeapon()->CanReload());
}

bool UWeaponHandlerComponent::AllowShooting() const
{
	return (IsAnyWeaponEquipped())
		&& (!IsPendingReload())
		&& (!IsEquippingAnyWeapon())
		&& (!IsUnequippingAnyWeapon())
		&& (GetEquippedWeapon()->CanFire());
}

void UWeaponHandlerComponent::SetState(EWeaponHoldingState NewState)
{
	WeaponHoldingState = NewState;
}

void UWeaponHandlerComponent::DrawWeaponSight()
{
	if (EquippedWeapon)
	{
		const FVector TraceStart = EquippedWeapon->GetMuzzleWorldLocation();

		DrawDebugLine(
			GetWorld(),
			TraceStart,
			TraceStart + EquippedWeapon->GetActorRightVector() * 1000.0f,
			FColor::Red,
			false,
			0.0f,
			'\000',
			2.0f
		);
	}
}

void UWeaponHandlerComponent::PrepareStartingWeapons()
{
	// Spawn Primary.
	if (!PrimaryStartingWeapon.IsNull())
	{
		TSubclassOf<AWeaponBase> WeaponClass = PrimaryStartingWeapon.LoadSynchronous();

		AWeaponBase* WeaponWithDeferredSpawn = GetWorld()->SpawnActorDeferred<AWeaponBase>(WeaponClass,
			FTransform::Identity,
			GetOwner(),
			CastChecked<APawn>(GetOwner()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		WeaponWithDeferredSpawn->SetPlayer(Cast<ATpsCharacterBase>(GetOwner()));

		PrimaryWeapon = WeaponWithDeferredSpawn;

		WeaponWithDeferredSpawn->FinishSpawning(FTransform::Identity);
	}
}

// Called when the game starts
void UWeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadAnimationMontages();

	CharacterOwner = Cast<ATpsCharacterBase>(GetOwner());
	CharacterMesh = CharacterOwner->GetMesh();

	PrepareStartingWeapons();
}

void UWeaponHandlerComponent::LoadAnimationMontages()
{
	FireSingleWeaponAM = FireSingleWeaponSoftPtr.LoadSynchronous();
	FireContinuousWeaponAM = FireContinuousWeaponSoftPtr.LoadSynchronous();
	ReloadWeaponAM = ReloadWeaponSoftPtr.LoadSynchronous();
	EquipWeaponAM = EquipWeaponSoftPtr.LoadSynchronous();
	UnequipWeaponAM = UnequipWeaponSoftPtr.LoadSynchronous();

	/*
	FireSingleWeaponAM->AddSlot("UpperBody");
	FireContinuousWeaponAM->AddSlot("UpperBody");
	ReloadWeaponAM->AddSlot("UpperBody");
	EquipWeaponAM->AddSlot("UpperBody");
	UnequipWeaponAM->AddSlot("UpperBody");
	*/

	RecoverMontageNotifications();
}

void UWeaponHandlerComponent::PlayShotMontage()
{
	// CastChecked<ATpsCharacterBase>(GetOwner())->PlayAnimMontage(FireSingleWeaponAM);
	CharacterOwner->PlayAnimMontage(FireSingleWeaponAM);
}

void UWeaponHandlerComponent::RecoverMontageNotifications()
{
	TArray<FAnimNotifyEventReference> Notifies;

	if (EquipWeaponAM)
	{
		EquipWeaponAM->GetAnimNotifies(0.0f, EquipWeaponAM->CalculateSequenceLength(), false, Notifies);

		for (FAnimNotifyEventReference Notification : Notifies)
		{
			if (Notification.GetNotify()->GetNotifyEventName() == FName("AnimNotify_GrabWeapon"))
			{
				EquipWeapon_GrabWeaponTime = Notification.GetNotify()->GetTime();
			}
		}
		Notifies.Empty();
	}
	check(EquipWeapon_GrabWeaponTime >= 0.0f);

	if (UnequipWeaponAM)
	{
		UnequipWeaponAM->GetAnimNotifies(0.0f, UnequipWeaponAM->CalculateSequenceLength(), false, Notifies);
		for (FAnimNotifyEventReference Notification : Notifies)
		{
			if (Notification.GetNotify()->GetNotifyEventName() == FName("AnimNotify_ReleaseWeapon"))
			{
				UnequipWeapon_ReleaseWeaponTime = Notification.GetNotify()->GetTime();
			}
		}
		Notifies.Empty();
	}
	check(UnequipWeapon_ReleaseWeaponTime >= 0.0f);
}

bool UWeaponHandlerComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (!IsValid(WeaponToEquip))
	{
		UE_LOG(LogTemp, Display, TEXT("X % - Couldn't equip weapon, the object is not valid"), TEXT(__FUNCTION__));
		return false;
	}

	// return if we try to equip the same weapon.
	if (EquippedWeapon != nullptr)
	{
		if (WeaponToEquip->GetUniqueID() == EquippedWeapon->GetUniqueID())
		{
			UE_LOG(LogTemp, Display, TEXT("X % - Couldn't equip weapon %s, this weapon is already equipped."), TEXT(__FUNCTION__), *WeaponToEquip->GetHumanReadableName());
			return false;
		}
	}

	if (IsEquippingAnyWeapon())
	{
		return false;
	}

	GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_WeaponAction);

	// Unequip currently held weapon.
	UnequipWeapon();

	ATpsCharacterBase* Owner = Cast<ATpsCharacterBase>(GetOwner());
	Owner->PlayAnimMontage(EquipWeaponAM);

	WeaponToEquip->EquipOn(Owner);
	OnEquipWeapon.Broadcast(WeaponToEquip);

	// EquipWeaponWeaponAM->AddSlot("UpperBody");  

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	TimerDel.BindUFunction(WeaponToEquip, FName("AttachToSocket"), CharacterMesh, GetWeaponInHandAttachPointOnCharacter());
	// Attach the weapon to it's socket after specified duration.
	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, EquipWeapon_GrabWeaponTime, false);

	// Start timer for OnEquipFinisehd.
	GetOwner()->GetWorldTimerManager().SetTimer(
		TimerHandle_WeaponAction,
		this,
		&UWeaponHandlerComponent::EquipFinished,
		EquipWeaponAM->GetPlayLength(),
		false
	);


	// Keep the pointer of the currently equipped weapon.
	EquippedWeapon = WeaponToEquip;

	UE_LOG(LogTemp, Display, TEXT("V %s - Equipping weapon %s"), TEXT(__FUNCTION__), *WeaponToEquip->GetHumanReadableName());

	return true;
}

void UWeaponHandlerComponent::AttachToPawnHand(AWeaponBase* Weapon)
{
	// Attach the weapon to the player socket.
	ATpsCharacterBase* Owner = Cast<ATpsCharacterBase>(GetOwner());

	FAttachmentTransformRules attachmentTransformRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		false
	);

	Weapon->AttachToComponent(Owner->GetMesh(), attachmentTransformRules, Owner->GetWeaponInHandAttachPointOnCharacter());
}

void UWeaponHandlerComponent::AttachToPawnHoslterSlot(AWeaponBase* Weapon)
{
	bool AttachSucceed = false;

	ATpsCharacterBase* Owner = Cast<ATpsCharacterBase>(GetOwner());

	switch (Weapon->GetWeaponSlot())
	{
	case EWeaponSlot::Rifle:
		// AttachSucceed = Weapon->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Owner->GetHolsterWeaponAttachPointOnCharacter());
		break;
	case EWeaponSlot::Handgun:
		unimplemented();
		break;
	default:
		Weapon->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Error, TEXT("X %s - No matching WeaponSlot for holstering. Check if %s have it's WeaponSlot defined."), TEXT(__FUNCTION__), *GetName());
	}

	if (!AttachSucceed)
	{
		UE_LOG(LogTemp, Error, TEXT("X %s - Attaching weapon to it's holster socket failed."), TEXT(__FUNCTION__));
	}
}


FVector UWeaponHandlerComponent::GetWeaponHandSocketOwnerSpace(uint8 IsBackHand) const
{
	if (EquippedWeapon == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector SocketLocation_WeaponSpace;
	FName SocketName;
	int32 HandBoneIndex;

	// Find the matching socket for the matching hand.
	if (IsBackHand)
	{
		SocketName = EquippedWeapon->GetWeaponLeftHandSocketName();
		HandBoneIndex = EquippedWeapon->GetMesh()->GetBoneIndex(RightHandBoneName);
	}
	else
	{
		SocketName = EquippedWeapon->GetWeaponRightHandSocketName();
		HandBoneIndex = EquippedWeapon->GetMesh()->GetBoneIndex(LeftHandBoneName);
	}

	const FTransform HandBoneTransform = EquippedWeapon->GetMesh()->GetBoneTransform(HandBoneIndex);

	SocketLocation_WeaponSpace = EquippedWeapon->GetMesh()->GetSocketLocation(SocketName);

	// Transform Location from Weapon Space to Hand Bone Space.
	FVector IK_HandLocation = HandBoneTransform.InverseTransformPosition(SocketLocation_WeaponSpace);

	return SocketLocation_WeaponSpace + GetOwner()->GetActorTransform().InverseTransformPosition(EquippedWeapon->GetActorLocation());
}

void UWeaponHandlerComponent::EquipPrimaryWeapon()
{
	if (CanSwitchWeapon())
	{
		if (IsPrimaryEquipped())
		{
			UnequipWeapon();
		}
		else
		{
			EquipWeapon(PrimaryWeapon);
		}
	}	
}

void UWeaponHandlerComponent::UnequipWeapon()
{
	bUseHandIK = false;

	if (EquippedWeapon)
	{
		if (IsPrimaryEquipped())
		{
			SetState(EWeaponHoldingState::UnequippingPrimary);
		}
		else
		{
			SetState(EWeaponHoldingState::Undefined);
			UE_LOG(LogTemp, Error, TEXT("X %s - Unequipping weapon %s, but it is neither the Primary or Secondary."), TEXT(__FUNCTION__), *EquippedWeapon->GetHumanReadableName());
		}
		UE_LOG(LogTemp, Display, TEXT("V %s - Unequipping weapon %s"), TEXT(__FUNCTION__), *EquippedWeapon->GetHumanReadableName());

		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_WeaponAction);

		EquippedWeapon->Unequip();
		OnUnequipWeapon.Broadcast(EquippedWeapon);

		FTimerDelegate TimerDel;
		FTimerHandle TimerHandle;

		TimerDel.BindUFunction(EquippedWeapon, FName("DetachFromPawn"), EquippedWeapon);
		// Detach the weapon to it's socket after specified duration.
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, UnequipWeapon_ReleaseWeaponTime, false);


		ATpsCharacterBase* Owner = Cast<ATpsCharacterBase>(GetOwner());
		Owner->PlayAnimMontage(UnequipWeaponAM);


		// Start timer for OnUnequipFinisehd.
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_WeaponAction,
			this,
			&UWeaponHandlerComponent::UnequipFinished,
			UnequipWeaponAM->GetPlayLength(),
			false
		);
	}
}

void UWeaponHandlerComponent::EquipFinished()
{
	EquippedWeapon->EquipFinished();
	OnEquipWeaponFinished.Broadcast(EquippedWeapon);

	SetState(EWeaponHoldingState::NoWeapon);

	if (EquippedWeapon == PrimaryWeapon)
	{
		SetState(EWeaponHoldingState::PrimaryIdle);
	}
	else
	{
		SetState(EWeaponHoldingState::Undefined);
	}

	EquippedWeapon->OnShot.AddUObject(this, &UWeaponHandlerComponent::PlayShotMontage);

	bUseHandIK = true;
}

void UWeaponHandlerComponent::UnequipFinished()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->UnequipFinished();
		OnUnequipWeaponFinished.Broadcast(EquippedWeapon);

		SetState(EWeaponHoldingState::NoWeapon);

		EquippedWeapon->OnShot.RemoveAll(this);

		EquippedWeapon = nullptr;
	}
}

void UWeaponHandlerComponent::Reload()
{
	if (!CanReload())
	{
		return;
	}

	if (EquippedWeapon)
	{
		if (EquippedWeapon->CanReload())
		{
			bUseHandIK = false;

			// Play the reload animation on the owner.
			ATpsCharacterBase* Owner = CastChecked<ATpsCharacterBase>(GetOwner());
			float AnimDuration = Owner->PlayAnimMontage(ReloadWeaponAM);

			SetState(EWeaponHoldingState::ReloadingPrimary);

			// Reload base on the anim duration.
			EquippedWeapon->Reload(AnimDuration);

			// Start timer for ReloadFinisehd.
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_WeaponAction,
				this,
				&UWeaponHandlerComponent::ReloadFinished,
				AnimDuration,
				false
			);
		}
	}
}

void UWeaponHandlerComponent::ReloadFinished()
{
	SetState(EWeaponHoldingState::PrimaryIdle);

	bUseHandIK = true;
}