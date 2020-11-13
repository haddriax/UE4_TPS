// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponHandlerComponent.generated.h"

class AWeaponBase;

/*
* WeaponHolding state.
*/
UENUM(BlueprintType)
enum class EWeaponHoldingState : uint8
{
	NoWeapon,
	PrimaryEquipped,
	SecondaryEquipped,
	EquippingPrimary,
	UnequippingPrimary,
	EquippingSecondary,
	UnequippingSecondary,
	Undefined
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4TPS_API UWeaponHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, AWeaponBase*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnEquipWeapon OnEquipWeapon;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnequipWeapon, AWeaponBase*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnUnequipWeapon OnUnequipWeapon;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeaponFinished, AWeaponBase*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnEquipWeaponFinished OnEquipWeaponFinished;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnequipFinished, AWeaponBase*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnUnequipFinished OnUnequipWeaponFinished;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> FireSingleWeaponSoftPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> FireContinuousWeaponSoftPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> ReloadWeaponSoftPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> EquipWeaponSoftPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> UnequipWeaponSoftPtr = nullptr;

	UAnimMontage* UnequipWeaponAM = nullptr;
	UAnimMontage* EquipWeaponAM = nullptr;
	UAnimMontage* ReloadWeaponAM = nullptr;
	UAnimMontage* FireContinuousWeaponAM = nullptr;
	UAnimMontage* FireSingleWeaponAM = nullptr;

	/*
	* The time after starting the EquipWeaponAM animation when the weapon must be attached to the socket.
	* Computed from the "GrabWeapon" AnimNotify.
	*/
	float EquipWeapon_GrabWeaponTime = -1.0f;

	/*
	* The time after starting the UnequipWeaponeWeaponAM animation when the weapon must be attached to the socket.
	* Computed from the "UnequipWeaponeWeaponAM" AnimNotify.
	*/
	float UnequipWeapon_ReleaseWeaponTime = -1.0f;

	/*
	* The IK virtual bone name from the Character mesh, used for the left hand.
	*/
	UPROPERTY(EditAnywhere, Category = "IK Weapon Handling")
		FName LeftHandIKBoneName = "ik_hand_l";

	/*
* The IK virtual bone name from the Character mesh, used for the right hand.
*/
	UPROPERTY(EditAnywhere, Category = "IK Weapon Handling")
		FName RightHandIKBoneName = "ik_hand_r";

	/*
	* The FK right hand name from the Character mesh.
	*/
	UPROPERTY(EditAnywhere, Category = "IK Weapon Handling")
		FName RightHandBoneName = "Hand_R";

	/*
	* The FK left hand name from the Character mesh.
	*/
	UPROPERTY(EditAnywhere, Category = "IK Weapon Handling")
		FName LeftHandBoneName = "Hand_L";

	UPROPERTY(EditAnywhere)
		FName WeaponAttachPointOnCharacter_Rifle = FName("middle_01_r_WeaponSocket");

	UPROPERTY(EditAnywhere)
		FName HolsterWeaponAttachPointOnCharacter_Rifle = FName("RifleHolsterSocket");

	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<AWeaponBase> PrimaryStartingWeapon;

	UPROPERTY(EditDefaultsOnly)
		TSoftClassPtr<AWeaponBase> SecondaryStartingWeapon;

	/*
	*
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AWeaponBase* PrimaryWeapon = nullptr;
	/*
	*
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AWeaponBase* SecondaryWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AWeaponBase* EquippedWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWeaponHoldingState WeaponHoldingState;

	USkeletalMeshComponent* CharacterMesh = nullptr;

	FTimerHandle TimerHandle_EquipWeapon;

	bool bUseHandIK = false;

public:
	// Sets default values for this component's properties.
	UWeaponHandlerComponent();

	// Called every frame.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Run whenever the Component is spawned or when a property changes.
	*/
	virtual void OnComponentCreated() override;

	/*
	* Is a weapon currently pending equip ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsEquippingAnyWeapon() const;

	/*
	* Is the Primary weapon currently equipped ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsPrimaryEquipped() const;

	/*
	* Is the Secondary weapon currently equipped ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsSecondaryEquipped() const;

	UFUNCTION(BlueprintPure, BlueprintCallable)
		FORCEINLINE bool ShouldUseHandIK() const { return bUseHandIK;  };

protected:
	/*
	* Instantiate the Starting Weapons and put them in the inventory.
	*/
	void PrepareStartingWeapons();

	// Called when the game starts.
	virtual void BeginPlay() override;

	void DeactivateHandIK() { bUseHandIK = false; };

	void DetermineWeaponHandlerState();

	/*
	* Load all the Animation Montage Soft Reference in memory.
	*/
	void LoadAnimationMontages();

	/*
	* Get the timestamps of the notification presents in the AnimMontages.
	*/
	void RecoverMontageNotifications();

	bool EquipWeapon(AWeaponBase* WeaponToEquip);


	void AttachToPawnHand(AWeaponBase* Weapon);

	void AttachToPawnHoslterSlot(AWeaponBase* Weapon);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AWeaponBase* GetPrimary() const { return PrimaryWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AWeaponBase* GetSecondary() const { return SecondaryWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetWeaponInHandAttachPointOnCharacter() const { return WeaponAttachPointOnCharacter_Rifle; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetHolsterWeaponAttachPointOnCharacter() const { return HolsterWeaponAttachPointOnCharacter_Rifle; }

	/*
	* Get the socket location on the weapon transformed to the Character space where the hands should go.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetWeaponHandSocketOwnerSpace(uint8 IsBackHand = 1) const;

	/*
	*
	*/
	void EquipPrimaryWeapon();

	void EquipSecondaryWeapon();

	/*
	* Start unequipping the weapon.
	*/
	void UnequipWeapon();

	/*
	* Called when weapon is finally equipped and ready to use.
	*/
	void EquipFinished();
	/*
	* Called when weapon is finally unequipped and other actions can be done.
	*/
	void UnequipFinished();

	void Reload();

	void ReloadFinished();
};
