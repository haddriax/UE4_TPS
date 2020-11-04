// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/WeaponHandlerComponent.h"

#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"
// #include "Engine/StreamableManager.h"

#include "Characters/Controllers/TpsPlayerController.h"
#include "Characters/TpsCharacter.h"
#include "Weapons/WeaponBase.h"

// Sets default values for this component's properties
UWeaponHandlerComponent::UWeaponHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called every frame
void UWeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// DEACTIVATED
}

void UWeaponHandlerComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}

bool UWeaponHandlerComponent::IsEquippingAnyWeapon() const
{
	return ((WeaponHoldingState == EWeaponHoldingState::EquippingPrimary) || (WeaponHoldingState == EWeaponHoldingState::EquippingSecondary));
}

bool UWeaponHandlerComponent::IsPrimaryEquipped() const
{
	return (WeaponHoldingState == EWeaponHoldingState::PrimaryEquipped);
}

bool UWeaponHandlerComponent::IsSecondaryEquipped() const
{
	return (WeaponHoldingState == EWeaponHoldingState::SecondaryEquipped);
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
			Cast<APawn>(GetOwner()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		WeaponWithDeferredSpawn->SetPlayer(Cast<ATpsCharacter>(GetOwner()));

		PrimaryWeapon = WeaponWithDeferredSpawn;

		WeaponWithDeferredSpawn->FinishSpawning(FTransform::Identity);
	}

	// Spawn Secondary.
	if (!SecondaryStartingWeapon.IsNull())
	{
		TSubclassOf<AWeaponBase> WeaponClass = SecondaryStartingWeapon.LoadSynchronous();

		AWeaponBase* WeaponWithDeferredSpawn = GetWorld()->SpawnActorDeferred<AWeaponBase>(WeaponClass,
			FTransform::Identity,
			GetOwner(),
			Cast<APawn>(GetOwner()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		WeaponWithDeferredSpawn->SetPlayer(Cast<ATpsCharacter>(GetOwner()));

		SecondaryWeapon = WeaponWithDeferredSpawn;
		WeaponWithDeferredSpawn->FinishSpawning(FTransform::Identity);
	}
}

// Called when the game starts
void UWeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadAnimationMontages();

	CharacterMesh = Cast<ACharacter>(GetOwner())->GetMesh();

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
		check(EquipWeapon_GrabWeaponTime >= 0.0f);
		Notifies.Empty();
	}



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
		check(UnequipWeapon_ReleaseWeaponTime >= 0.0f);
		Notifies.Empty();
	}
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

	// Unequip currently held weapon.
	UnequipWeapon();

	ATpsCharacter* Owner = Cast<ATpsCharacter>(GetOwner());
	Owner->PlayAnimMontage(EquipWeaponAM);

	WeaponToEquip->EquipOn(Owner);
	OnEquipWeapon.Broadcast(WeaponToEquip);

	// EquipWeaponWeaponAM->AddSlot("UpperBody");  

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	TimerDel.BindUFunction(WeaponToEquip, FName("AttachToSocket"), CharacterMesh, GetWeaponInHandAttachPointOnCharacter());
	// Attach the weapon to it's socket after specified duration.
	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, EquipWeapon_GrabWeaponTime, false);

	// Wait a bit of time after the animation end.
	float constexpr EquipAnimationSafeMargin = 0.08f;

	// Start timer for OnEquipFinisehd.
	GetOwner()->GetWorldTimerManager().SetTimer(
		TimerHandle_EquipWeapon,
		this,
		&UWeaponHandlerComponent::EquipFinished,
		EquipWeaponAM->GetPlayLength() + EquipAnimationSafeMargin,
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
	ATpsCharacter* Owner = Cast<ATpsCharacter>(GetOwner());

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

	ATpsCharacter* Owner = Cast<ATpsCharacter>(GetOwner());

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

	// WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	// WeaponMesh->SetHiddenInGame(false);
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

	FTransform const HandBoneTransform = EquippedWeapon->GetMesh()->GetBoneTransform(HandBoneIndex);

	SocketLocation_WeaponSpace = EquippedWeapon->GetMesh()->GetSocketLocation(SocketName);

	// Transform Location from Weapon Space to Hand Bone Space.
	FVector IK_HandLocation = HandBoneTransform.InverseTransformPosition(SocketLocation_WeaponSpace);

	return SocketLocation_WeaponSpace + GetOwner()->GetActorTransform().InverseTransformPosition(EquippedWeapon->GetActorLocation());
}

void UWeaponHandlerComponent::EquipPrimaryWeapon()
{
	if (IsPrimaryEquipped() || IsSecondaryEquipped())
	{
		UnequipWeapon();
	}
	else
	{
		if (EquipWeapon(PrimaryWeapon))
		{
			WeaponHoldingState = EWeaponHoldingState::EquippingPrimary;
		}
	}
}

void UWeaponHandlerComponent::EquipSecondaryWeapon()
{
	if (IsPrimaryEquipped() || IsSecondaryEquipped())
	{
		UnequipWeapon();
	}
	if (EquipWeapon(SecondaryWeapon))
	{
		WeaponHoldingState = EWeaponHoldingState::EquippingSecondary;
	}

}

void UWeaponHandlerComponent::UnequipWeapon()
{
	bUseHandIK = false;

	if (EquippedWeapon)
	{
		EquippedWeapon->Unequip();
		OnUnequipWeapon.Broadcast(EquippedWeapon);

		if (WeaponHoldingState == EWeaponHoldingState::PrimaryEquipped)
		{
			WeaponHoldingState = EWeaponHoldingState::UnequippingPrimary;
		}
		else if (WeaponHoldingState == EWeaponHoldingState::SecondaryEquipped)
		{
			WeaponHoldingState = EWeaponHoldingState::UnequippingSecondary;
		}
		else
		{
			WeaponHoldingState = EWeaponHoldingState::Undefined;
			UE_LOG(LogTemp, Error, TEXT("X %s - Unequipping weapon %s, but it is neither the Primary or Secondary."), TEXT(__FUNCTION__), *EquippedWeapon->GetHumanReadableName());
		}

		FTimerDelegate TimerDel;
		FTimerHandle TimerHandle;

		TimerDel.BindUFunction(EquippedWeapon, FName("DetachFromPawn"), EquippedWeapon);
		// Detach the weapon to it's socket after specified duration.
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, UnequipWeapon_ReleaseWeaponTime, false);


		ATpsCharacter* Owner = Cast<ATpsCharacter>(GetOwner());
		Owner->PlayAnimMontage(UnequipWeaponAM);

		UE_LOG(LogTemp, Display, TEXT("V %s - Unequipping weapon %s"), TEXT(__FUNCTION__), *EquippedWeapon->GetHumanReadableName());

		// Wait a bit of time after the animation end.
		float constexpr UnequipAnimationSafeMargin = 0.08f;

		// Start timer for OnUnequipFinisehd.
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_EquipWeapon,
			this,
			&UWeaponHandlerComponent::UnequipFinished,
			UnequipWeaponAM->GetPlayLength() + UnequipAnimationSafeMargin,
			false
		);
	}
}

void UWeaponHandlerComponent::EquipFinished()
{
	EquippedWeapon->EquipFinished();
	OnEquipWeaponFinished.Broadcast(EquippedWeapon);

	WeaponHoldingState = EWeaponHoldingState::NoWeapon;

	if (EquippedWeapon == PrimaryWeapon)
	{
		WeaponHoldingState = EWeaponHoldingState::PrimaryEquipped;
	}
	else if (EquippedWeapon == SecondaryWeapon)
	{
		WeaponHoldingState = EWeaponHoldingState::SecondaryEquipped;
	}
	else
	{
		WeaponHoldingState = EWeaponHoldingState::Undefined;
	}

	bUseHandIK = true;
}

void UWeaponHandlerComponent::UnequipFinished()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->UnequipFinished();
		OnUnequipWeaponFinished.Broadcast(EquippedWeapon);

		WeaponHoldingState = EWeaponHoldingState::NoWeapon;

		EquippedWeapon = nullptr;
	}
}

void UWeaponHandlerComponent::Reload()
{
	if (EquippedWeapon)
	{
		bUseHandIK = false;
		// Since CanRelaod() is also call in EquippedWeapon->Reload(), may need refactoring.
		if (EquippedWeapon->CanReload())
		{

			// Play the reload animation on the owner.
			ATpsCharacter* Owner = Cast<ATpsCharacter>(GetOwner());
			float AnimDuration = Owner->PlayAnimMontage(ReloadWeaponAM);

			// Reload base on the anim duration.
			EquippedWeapon->Reload(AnimDuration);

			// Start timer for ReloadFinisehd.
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_EquipWeapon,
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
	bUseHandIK = true;
}

