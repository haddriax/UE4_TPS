// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../GameTypes.h"

#include "CharacterWeaponComponent.generated.h"

class AModularWeapon;
class ATpsCharacterBase;
class UAnimMontage;

/*
* WeaponHolding state.
*/
UENUM(BlueprintType)
enum class EWeaponHoldingState : uint8
{
	NoWeapon,
	PrimaryIdle,
	EquippingPrimary,
	UnequippingPrimary,
	ReloadingPrimary,
	Undefined
};


/*
* Usable on any childclass of ATpsCharacterBase.
* Allow storage and weapon usage.
* Actions : Equip & Unequip / Reload / Fire
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4TPS_API UCharacterWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, AModularWeapon*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnEquipWeapon OnEquipWeapon;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnequipWeapon, AModularWeapon*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnUnequipWeapon OnUnequipWeapon;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeaponFinished, AModularWeapon*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnEquipWeaponFinished OnEquipWeaponFinished;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnequipFinished, AModularWeapon*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnUnequipFinished OnUnequipWeaponFinished;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReload, AModularWeapon*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnReload OnReload;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadFinished, AModularWeapon*, Weapon);

	UPROPERTY(BlueprintAssignable)
		FOnReloadFinished OnReloadFinished;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> ReloadWeaponSoftPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> EquipWeaponSoftPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<UAnimMontage> UnequipWeaponSoftPtr = nullptr;

	/*
	* The time after starting the EquipWeaponAM animation when the weapon must be attached to the socket.
	* Computed from the "GrabWeapon" AnimNotify.
	*/
	float EquipWeapon_GrabWeaponTime = -1.0f;

	/*
	* The time after starting the UnequipWeaponeWeaponAM animation when the weapon must be attached to the socket.
	* Computed from the "UnequipWeapon" AnimNotify.
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
		TSoftClassPtr<AModularWeapon> PrimaryStartingWeapon;

	/*
	*
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AModularWeapon* PrimaryWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AModularWeapon* EquippedWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWeaponHoldingState WeaponHoldingState;

	USkeletalMeshComponent* CharacterMesh = nullptr;

	ATpsCharacterBase* CharacterOwner = nullptr;

	FTimerHandle TimerHandle_WeaponAction;

	bool bUseHandIK = false;

	uint8 bIsPlayingContinuousFireMontage = false;

public:
	// Sets default values for this component's properties.
	UCharacterWeaponComponent();

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
	* Is a weapon currently pending unequip ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsUnequippingAnyWeapon() const;

	/*
	* Is any weapon currently equipped in hands (and not pending Unequip) ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsAnyWeaponEquipped() const;

	/*
	* Is the Primary weapon currently equipped ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsPrimaryEquipped() const;

	/*
	* Is equippied weapon pending reload ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool IsPendingReload() const;

	/*
	* Can we currently Equip/Unequip the weapon ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool CanSwitchWeapon() const;

	/*
	* Can we currently reload the equipped weapon ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool CanReload() const;

	/*
	* Can we actually shoot with the equipped weapon ?
	*/
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool AllowShooting() const;

	UFUNCTION(BlueprintPure, BlueprintCallable)
		FORCEINLINE bool ShouldUseHandIK() const { return bUseHandIK; };

protected:
	void SetState(EWeaponHoldingState NewState);

	void DrawWeaponSight();

	/*
	* Instantiate the Starting Weapons and put them in the inventory.
	*/
	void PrepareStartingWeapons();

	// Called when the game starts.
	virtual void BeginPlay() override;

	void DeactivateHandIK() { bUseHandIK = false; };

	/*
	* Load all the Animation Montage Soft Reference in memory.
	*/
	void LoadAnimationMontages();

	UFUNCTION()
		void PlayShotMontageSingle();

	UFUNCTION()
		void TogglePlayShotMontageLoop();
	/*
	* Get the timestamps of the notification presents in the AnimMontages.
	*/
	void RecoverMontageNotifications();

	bool EquipWeapon(AModularWeapon* WeaponToEquip);


	UFUNCTION(BlueprintCallable)
		void AttachToPawnHand(AModularWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
		void AttachToPawnHoslterSlot(AModularWeapon* Weapon);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		AModularWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AModularWeapon* GetPrimary() const { return PrimaryWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetWeaponInHandAttachPointOnCharacter() const { return WeaponAttachPointOnCharacter_Rifle; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetHolsterWeaponAttachPointOnCharacter() const { return HolsterWeaponAttachPointOnCharacter_Rifle; }

	/*
	* Get the socket location on the weapon transformed to the Character space where the hands should go.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetWeaponHandSocketOwnerSpace(uint8 IsBackHand = 1) const;

#pragma region CALLABLE_ACTIONS
	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void EquipPrimaryWeapon();

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
#pragma endregion CALLABLE_ACTIONS
