// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/CharacterWeaponComponent.h"

#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
// #include "Engine/StreamableManager.h"

#include "Characters/Controllers/TpsPlayerController.h"
#include "Characters/TpsCharacterBase.h"
#include "Weapons/ModularWeapon.h"
#include "Weapons/FireMode/WeaponFireModeComponent.h"

// Sets default values for this component's properties
UCharacterWeaponComponent::UCharacterWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	bIsPlayingContinuousFireMontage = false;
}

// Called every frame
void UCharacterWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	DrawWeaponSight();
}

void UCharacterWeaponComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	SetState(EWeaponHoldingState::NoWeapon);
}

bool UCharacterWeaponComponent::IsEquippingAnyWeapon() const
{
	return (WeaponHoldingState == EWeaponHoldingState::EquippingPrimary);
}

bool UCharacterWeaponComponent::IsUnequippingAnyWeapon() const
{
	return (WeaponHoldingState == EWeaponHoldingState::UnequippingPrimary);
}

bool UCharacterWeaponComponent::IsAnyWeaponEquipped() const
{
	return ((EquippedWeapon != nullptr) && (WeaponHoldingState != EWeaponHoldingState::UnequippingPrimary));
}

bool UCharacterWeaponComponent::IsPrimaryEquipped() const
{
	return (WeaponHoldingState == EWeaponHoldingState::PrimaryIdle);
}

bool UCharacterWeaponComponent::IsPendingReload() const
{
	return (WeaponHoldingState == EWeaponHoldingState::ReloadingPrimary);
}

bool UCharacterWeaponComponent::CanSwitchWeapon() const
{
	return (!IsEquippingAnyWeapon())
		&& (!IsUnequippingAnyWeapon())
		&& (!IsPendingReload());
}

bool UCharacterWeaponComponent::CanReload() const
{
	if (!GetEquippedWeapon())
		return false;
	else
		return (!IsPendingReload())
		&& (!IsEquippingAnyWeapon())
		&& (!IsUnequippingAnyWeapon())
		&& (GetEquippedWeapon()->CanReload());
}

bool UCharacterWeaponComponent::AllowShooting() const
{
	return (IsAnyWeaponEquipped())
		&& (!IsPendingReload())
		&& (!IsEquippingAnyWeapon())
		&& (!IsUnequippingAnyWeapon())
		&& (GetEquippedWeapon()->CanFire());
}

void UCharacterWeaponComponent::SetState(EWeaponHoldingState NewState)
{
	WeaponHoldingState = NewState;
}

void UCharacterWeaponComponent::DrawWeaponSight()
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

void UCharacterWeaponComponent::PrepareStartingWeapons()
{
	// Spawn Primary.
	if (!PrimaryStartingWeapon.IsNull())
	{
		TSubclassOf<AModularWeapon> WeaponClass = PrimaryStartingWeapon.LoadSynchronous();

		AModularWeapon* WeaponWithDeferredSpawn = GetWorld()->SpawnActorDeferred<AModularWeapon>(WeaponClass,
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
void UCharacterWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadAnimationMontages();

	CharacterOwner = Cast<ATpsCharacterBase>(GetOwner());
	check(CharacterOwner);

	CharacterMesh = CharacterOwner->GetMesh();

	PrepareStartingWeapons();
}

void UCharacterWeaponComponent::LoadAnimationMontages()
{
	FireSingleWeaponSoftPtr.LoadSynchronous();
	FireContinuousWeaponSoftPtr.LoadSynchronous();
	ReloadWeaponSoftPtr.LoadSynchronous();
	EquipWeaponSoftPtr.LoadSynchronous();
	UnequipWeaponSoftPtr.LoadSynchronous();

	/*
	FireSingleWeaponAM->AddSlot("UpperBody");
	FireContinuousWeaponAM->AddSlot("UpperBody");
	ReloadWeaponAM->AddSlot("UpperBody");
	EquipWeaponAM->AddSlot("UpperBody");
	UnequipWeaponAM->AddSlot("UpperBody");
	*/

	RecoverMontageNotifications();
}

void UCharacterWeaponComponent::PlayShotMontageSingle()
{
	CharacterOwner->PlayAnimMontage(FireSingleWeaponSoftPtr.Get());
}

void UCharacterWeaponComponent::TogglePlayShotMontageLoop()
{
	if (bIsPlayingContinuousFireMontage)
	{
		CharacterOwner->StopAnimMontage(FireContinuousWeaponSoftPtr.Get());
	}
	else
	{
		CharacterOwner->PlayAnimMontage(FireContinuousWeaponSoftPtr.Get());
	}
	
	bIsPlayingContinuousFireMontage = !bIsPlayingContinuousFireMontage;
	
}

void UCharacterWeaponComponent::RecoverMontageNotifications()
{
	TArray<FAnimNotifyEventReference> Notifies;

	if (EquipWeaponSoftPtr.Get())
	{
		EquipWeaponSoftPtr.Get()->GetAnimNotifies(0.0f, EquipWeaponSoftPtr.Get()->CalculateSequenceLength(), false, Notifies);

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

	if (UnequipWeaponSoftPtr.Get())
	{
		UnequipWeaponSoftPtr.Get()->GetAnimNotifies(0.0f, UnequipWeaponSoftPtr.Get()->CalculateSequenceLength(), false, Notifies);
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

bool UCharacterWeaponComponent::EquipWeapon(AModularWeapon* WeaponToEquip)
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

	ATpsCharacterBase* Owner = CastChecked<ATpsCharacterBase>(GetOwner());
	Owner->PlayAnimMontage(EquipWeaponSoftPtr.Get());

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
		&UCharacterWeaponComponent::EquipFinished,
		EquipWeaponSoftPtr.Get()->GetPlayLength(),
		false
	);

	// Keep the pointer of the currently equipped weapon.
	EquippedWeapon = WeaponToEquip;

	UE_LOG(LogTemp, Display, TEXT("V %s - Equipping weapon %s"), TEXT(__FUNCTION__), *WeaponToEquip->GetHumanReadableName());

	return true;
}

void UCharacterWeaponComponent::AttachToPawnHand(AModularWeapon* Weapon)
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

void UCharacterWeaponComponent::AttachToPawnHoslterSlot(AModularWeapon* Weapon)
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


FVector UCharacterWeaponComponent::GetWeaponHandSocketOwnerSpace(uint8 IsBackHand) const
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

void UCharacterWeaponComponent::EquipPrimaryWeapon()
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

void UCharacterWeaponComponent::UnequipWeapon()
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

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(EquippedWeapon, FName("DetachFromPawn"), EquippedWeapon);
		// Detach the weapon to it's socket after specified duration.
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, UnequipWeapon_ReleaseWeaponTime, false);

		ATpsCharacterBase* Owner = CastChecked<ATpsCharacterBase>(GetOwner());
		Owner->PlayAnimMontage(UnequipWeaponSoftPtr.Get());


		// Start timer for OnUnequipFinisehd.
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_WeaponAction,
			this,
			&UCharacterWeaponComponent::UnequipFinished,
			UnequipWeaponSoftPtr.Get()->GetPlayLength(),
			false
		);
	}
}

void UCharacterWeaponComponent::EquipFinished()
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

	// EquippedWeapon->OnFireStart.AddUObject(this, &UCharacterWeaponComponent::TogglePlayShotMontageLoop);
	// EquippedWeapon->OnFireStop.AddUObject(this, &UCharacterWeaponComponent::TogglePlayShotMontageLoop);

	for (UWeaponFireModeComponent* Mode : EquippedWeapon->GetFireModes())
	{
		Mode->OnFireStart.AddUObject(this, &UCharacterWeaponComponent::TogglePlayShotMontageLoop);
		Mode->OnFireStop.AddUObject(this, &UCharacterWeaponComponent::TogglePlayShotMontageLoop);
	}

	bUseHandIK = true;
}

void UCharacterWeaponComponent::UnequipFinished()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->UnequipFinished();
		OnUnequipWeaponFinished.Broadcast(EquippedWeapon);

		SetState(EWeaponHoldingState::NoWeapon);

		// EquippedWeapon->OnShot.RemoveAll(this); 
		// EquippedWeapon->OnFireStart.RemoveAll(this);
		// EquippedWeapon->OnFireStop.RemoveAll(this); 

		for (UWeaponFireModeComponent* Mode : EquippedWeapon->GetFireModes())
		{
			Mode->OnShot.RemoveAll(this);
			Mode->OnFireStart.RemoveAll(this);
			Mode->OnFireStop.RemoveAll(this);
		}

		EquippedWeapon = nullptr;
	}
}

void UCharacterWeaponComponent::Reload()
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
			float AnimDuration = Owner->PlayAnimMontage(ReloadWeaponSoftPtr.Get());

			SetState(EWeaponHoldingState::ReloadingPrimary);

			// Reload based on the anim duration.
			EquippedWeapon->Reload(AnimDuration);

			// Start timer for ReloadFinisehd.
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_WeaponAction,
				this,
				&UCharacterWeaponComponent::ReloadFinished,
				AnimDuration,
				false
			);
		}
	}
}

void UCharacterWeaponComponent::ReloadFinished()
{
	SetState(EWeaponHoldingState::PrimaryIdle);

	bUseHandIK = true;
}