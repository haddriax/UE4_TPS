// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../GameTypes.h"

#include "WeaponBase.generated.h"

class UAnimMontage;
class USoundCue;
class UAudioComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USkeletalMeshComponent;
class ATpsCharacterBase;

class UWeaponFeedbacksComponent;

/*
* State of a weapon.
*/
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Reloading,
	Equipping,
	Unequipping
};

namespace EWeaponFireType
{
	enum Type
	{
		Hitscan,
		Projectile
	};
}

/*
* Slot taking by this weapon in inventory.
* Responsible for the matching holster socket on the Character.
* Any modifications here need to be reported to the Unequip method.
*/
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Rifle     UMETA(DisplayName = "Rifle"),
	Handgun   UMETA(DisplayName = "Handgun"),
	Undefined UMETA(DisplayName = "Undefined"),
};

/*
* Struct used to hold the static datas from this weapon.
*/
USTRUCT()
struct FWeaponDatas
{
	GENERATED_BODY();

	/*
	* Define then handling of the Weapon.
	*/
	UPROPERTY(EditDefaultsOnly)
		EWeaponType WeaponType = EWeaponType::Undefined;

	/*
	* Where the weapon will be located when holstered.
	*/
	UPROPERTY(EditDefaultsOnly)
		EWeaponSlot WeaponSlot = EWeaponSlot::Undefined;

	/*
	* Max ammunition in reserve for this weapon.
	*/
	UPROPERTY(EditDefaultsOnly, Category = Ammunitions)
		int32 MaxAmmunitions;

	/*
	* Number of ammunitions available in one full clip.
	*/
	UPROPERTY(EditDefaultsOnly, Category = Ammunitions)
		int32 AmmunitionsPerClip;

	/*
	* Number of clips available when this weapon is spawned.
	*/
	UPROPERTY(EditDefaultsOnly, Category = Ammunitions)
		int32 InitialClips;

	/*
	* Damage dealt per bullet.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float Damage;

	/*
	* Bullets shots every second of holding fire.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float RateOfFire;

	/*
	* The biggest angle in degrees a shot can be deviated due to accuracy loss.
	* 0 mean accuracy is always perfect.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float MaxSpreadAngle;

	/*
	* The smallest angle in degrees a shot can be deviated due to accuracy loss when the weapon is at it's perfect accuracy.
	* Can not be bigger than MaxSpreadAngle.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float MinSpreadAngle;

	/*
	* Spread added every time is a bullet is fired
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float ShotForMaxSpread;

	/*
	* TO IMPLEMENT.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float SpreadRecovery;

	/*
	* Time needed to raise the weapon, disabling the weapon meanwhile.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float EquipDuration;

	/*
	* Time needed for a full reload, disabling the weapon meanwhile.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float ReloadDuration;

	/*
	* Automtic fire or single shot.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		bool bIsAutomaticWeapon;

	/*
	* Defaults values.
	*/
	FWeaponDatas()
	{
		AmmunitionsPerClip = 30;
		InitialClips = 4;
		MaxAmmunitions = 300;
		EquipDuration = 1.0f;
		ReloadDuration = 1.0f;
		MinSpreadAngle = 0.0f;
		MaxSpreadAngle = 0.0f;
		ShotForMaxSpread = 2.0f;
		SpreadRecovery = 10.0f;
		Damage = 10.0f;
		RateOfFire = 5.0f;
		bIsAutomaticWeapon = true;
	}

};

/**
 * Base class for any usable weapon.
 */
UCLASS(Abstract, Blueprintable)
class UE4TPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FOnFireStop);
	FOnFireStop OnFireStop;

	DECLARE_DELEGATE(FOnFireStart);
	FOnFireStart OnFireStart;

	DECLARE_MULTICAST_DELEGATE(FOnShot);
	FOnShot OnShot;

private:

	/*
	* The Character which got this Weapon attach to.
	*/
	ATpsCharacterBase* ParentCharacter = nullptr;

protected:
	UPROPERTY(VisibleAnywhere)
		UWeaponFeedbacksComponent* WeaponFeedbacksComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets|IK")
		FName WeaponRightHandSocketName = "socket_hand_right";

	UPROPERTY(EditDefaultsOnly, Category = "Sockets|IK")
		FName WeaponLeftHandSocketName = "socket_hand_left";

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName MuzzleFX_SocketName = "MuzzleSocket";

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName MuzzleTrace_SocketName = "MuzzleSocket";

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName MuzzleDirection_SocketName = "MuzzleDirectionSocket";

	bool bIsEquipped = false;
	bool bRefiring = false;

	bool bIsPlayingFireAnimation = false;

	/*
	* Amount of bullet shot this burst (while Fire order is maintained).
	*/
	int32 BurstCounter = 0;

	/*
	* Game Time when the last bullet was fired. Used for the Fire timer.
	*/
	float LastFireTime = 0.0f;

	/*
	* Time between each shot. Computed from the RateOfFire in WeaponDatas.
	*/
	float TimeBetweenShots = 1.0f;

	/*
	* Current Accuracy modifier, from 0 to 1 where 0 is the weapon best accuracy, 1 is the weapon worst accuracy.
	*/
	float CurrentSpread = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Weapon")
		int32 AmmunitionsReserve = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Weapon")
		int32 AmmunitionsInClip = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWeaponState CurrentState;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Configs")
		FWeaponDatas WeaponDatas;

	// Timers Handle
	FTimerHandle TimerHandle_HandleFiring;
	FTimerHandle TimerHandle_WeaponAction;

public:
	/*
	* Return the name of the socket where the firing hand should be.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetWeaponRightHandSocketName() const { return WeaponRightHandSocketName; }

	/*
	* Return the socket name where the gripping hand should be.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetWeaponLeftHandSocketName() const { return WeaponLeftHandSocketName; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetShotDirectionSocketName() const { return WeaponLeftHandSocketName; }

	/*
	* Return the type of slot this weapon is using to be holstered.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE EWeaponSlot GetWeaponSlot() const { return WeaponDatas.WeaponSlot; }

	/*
	*Return the type of slot this weapon is using to be holstered.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE EWeaponState GetWeaponState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetDamage() const { return WeaponDatas.Damage; }

	AWeaponBase();

	virtual void PostInitializeComponents() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

protected:
	/*
	* Attach the weapon to the owning player hand, and snap it to the weapon socket.
	*/
	UFUNCTION()
		void AttachToPawnHand(ATpsCharacterBase* Character);

	/*
	* Attach the weapon to the owning player dedicated holster slot.
	*/
	void AttachToPawnHoslterSlot(ATpsCharacterBase* Character);

	UFUNCTION()
		/*
		* Attach and snap this weapon to a socket.
		* @param The USkeletalMeshComponent where we want to attach the weapon.
		* @param The socket name where the weapon should snap.
		* @return True - The attachment was successful.
		*/
		bool AttachToSocket(USkeletalMeshComponent* MeshToAttachOn, FName SocketName);

	/*
	* Detach the weapon from it's owner and hide it.
	*/
	UFUNCTION()
		void DetachFromPawn();

	/*
	* Manage timer for shots.
	*/
	void OnBurstStarted();

	/*
	* After a burst.
	*/
	void OnBurstEnded();

	/*
	* Weapon in world effect implementation. Must be overridden by WeaponBase child classes.
	*/
	virtual void FireWeapon();

	/*
	* Called on each shot of a burst.
	*/
	void HandleFiring();

	/*
	* Called before HandleFiring for consecutive shots of a burst.
	*/
	void HandleRefiring();

	/*
	* Given a Direction and two angle, rotate this direction randomly in a cone.
	* @param IN/OUT FVector : The initial direction we want to randomize.
	* @param float : Horizontal angle used to delimited the cone, in degrees.
	* @param float : Vertical angle used to delimited the cone, in degrees.
	* @param bool : Are the specified angles in radiant or degrees ?
	* @return int32 : The RandomSeed used to get generate the new direction.
	*/
	int32 AddRandomDirectionFromCone(FVector& DirectionToModify, float HorizontalAngle, float VerticalAngle, bool bAreRadiantAngles = true);

public:
	const FName GetMuzzleAttachPoint() const;

	void SetPlayer(class ATpsCharacterBase* _Player);

	/*
	* Set a new state and call the needed methods when switching states.
	*/
	void SetWeaponState(EWeaponState NewState);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		float GetEquipDuration() const { return WeaponDatas.EquipDuration; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		float GetTimeBetweenShots() const { return TimeBetweenShots; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		bool IsWeaponEquipped() const { return bIsEquipped; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		bool IsIdling() const { return CurrentState == EWeaponState::Idle; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		bool IsPendingReload() const { return CurrentState == EWeaponState::Reloading; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		bool IsPendingEquip() const { return CurrentState == EWeaponState::Equipping; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		bool IsPendingUnequip() const { return CurrentState == EWeaponState::Unequipping; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		bool IsFiring() const { return CurrentState == EWeaponState::Firing; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		int32 GetMaxAmmunitionsReserve() const { return WeaponDatas.MaxAmmunitions; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		int32 GetCurrentAmmunitionsReserve() const { return AmmunitionsReserve; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		int32 GetClipSize() const { return WeaponDatas.AmmunitionsPerClip; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		int32 GetAmmunitionInClip() const { return AmmunitionsInClip; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		float GetCurrentSpread() const { return CurrentSpread; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		float GetMaxSpread() const { return WeaponDatas.MaxSpreadAngle; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		float GetRateOfFire() const { return WeaponDatas.RateOfFire; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		ATpsCharacterBase* GetParentCharacter() const { return ParentCharacter; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		USkeletalMeshComponent* GetMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		FVector GetMuzzleWorldLocation() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		FVector GetShotWorldDirection() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		EWeaponType GetWeaponType() const { return  WeaponDatas.WeaponType; }

	/*
	* Ask this weapon to start firing, if possible.
	*/
	void TryShooting();

	/*
	* Ask this weapon to stop firing, if possible.
	*/
	void StopShooting();

	/*
	* @return True - The weapon can currently fire.
	*/
	bool CanFire() const;

	/*
	* @return True - The weapon can currently be reloaded.
	*/
	bool CanReload() const;

	/*
	* @return True - The player can either Draw or Holster it's weapon.
	*/
	bool CanSwitchWeapon() const;

	/*
	* Take this weapon in hand.
	* @return True - Equip can be executed.
	*/
	bool EquipOn(ATpsCharacterBase* Character);

	/*
	* Called once full equip animation is finished by WeaponHandlerComponent.
	*/
	void EquipFinished();

	/*
	* Holster this weapon.
	* @return True - Unequip can be executed.
	*/
	bool Unequip();

	/*
	* Called once full unequip animation is finished by WeaponHandlerComponent.
	*/
	void UnequipFinished();

	/*
	* Reload this weapon.
	*/
	void Reload(float ReloadDuration);

	/*
	* Called once the reload is finished.
	*/
	void OnReloadFinished();

	/*
	* Get the maximum of Amount from the ammo reserve.
	* @param : The amount of ammunitions we want to pull from the reserve.
	* @return : The amount of ammunitions that has been taken from reserve.
	*/
	int32 GetAmmuntionsFromReserve(int32 Amount);

	/*
	* Get the amount of ammunitions missing comparing to the maximal clip size.
	* @return : How much ammunitions are needed to fulfill the current clip.
	*/
	int32 GetMissingAmmoInClip() const;

	/*
	* Add the amount of ammunitions in the weapon clip, clamped by its max size.
	*/
	void AddAmmuntionsInClip(int32 Amount);

	/*
	* Consume one ammunition from the current clip. Use when shooting.
	*/
	void ConsumeAmmunition();

	/*
	* Increase the current spread after a bullet is shot.
	*/
	void IncreaseSpread();

	/*
	* Instantly reset the spread to 0.
	*/
	void ResetSpread();
};
