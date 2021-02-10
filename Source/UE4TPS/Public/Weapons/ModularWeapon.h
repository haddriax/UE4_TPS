// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../GameTypes.h"

#include "ModularWeapon.generated.h"

class USkeletalMeshComponent;
class ATpsCharacterBase;
class UAnimMontage;

class UWeaponFireModeComponent;

/*
* Struct used to hold the static datas from this weapon.
*/
USTRUCT(BlueprintType)
struct FModularWeaponDatas
{
	GENERATED_BODY();

	/*
	* Define the handling of the Weapon.
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
	* Reload Animation playable on the Character holding this weapon.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponAnimations)
	UAnimMontage* AM_Reload;
	/*
	* Equip Animation playable on the Character holding this weapon.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponAnimations)
	UAnimMontage* AM_Equip;
	/*
	* Unequip Animation playable on the Character holding this weapon.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponAnimations)
	UAnimMontage* AM_Unequip;
	/*
	* Recoil Animation playable on the Character holding this weapon.
	*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponAnimations)
	UAnimMontage* AM_Recoil;

	/*
	* Defaults values.
	*/
	FModularWeaponDatas()
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
	}
};

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


/**
 * Base class for any usable weapon.
 * Fire behaviors are created by adding child classes of UWeaponFireModeComponent.
 * AModularWeapon can subscribe to it's UWeaponFireModeComponent events.
 */
UCLASS(Abstract, BlueprintType)
class UE4TPS_API AModularWeapon : public AActor
{
	GENERATED_BODY()

public:


private:

	/*
	* The Character which got this Weapon attach to.
	*/
	ATpsCharacterBase* ParentCharacter = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite, Transient, Category = "Weapon")
		TArray<UWeaponFireModeComponent*> FireModes;

	int32 ActiveFireMode = 0;

/**
* Convert those properties to Static, since they are the same for every weapon ?
*/
#pragma region Sockets
	UPROPERTY(EditDefaultsOnly, Category = "Sockets|IK")
		FName WeaponRightHandSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets|IK")
		FName WeaponLeftHandSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName MuzzleFX_SocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName MuzzleTrace_SocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName MuzzleDirection_SocketName;
#pragma endregion Sockets

	bool bIsEquipped = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWeaponState CurrentState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EWeaponType WeaponType = EWeaponType::Rifle;

	/*
	* Slot when weapon is holstered.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EWeaponSlot WeaponSlot = EWeaponSlot::Rifle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EAmmoType AmmunitionsUsed = EAmmoType::Rifle;

	/*
	* Timer for Reloading/Equipping/Unequipping/etc...
	*/
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

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetMuzzleFX_SocketName() const { return MuzzleFX_SocketName; }

	/*
	* Return the type of slot this weapon is using to be holstered.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE EWeaponSlot GetWeaponSlot() const { return WeaponSlot; }

	/*
	* @return : EWeaponState The actual state of the weapon.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE EWeaponState GetWeaponState() const { return CurrentState; }

	/*
	* Return the FireModes array.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE TArray<UWeaponFireModeComponent*> GetFireModes() const { return FireModes; }

	AModularWeapon();

	virtual void PostInitializeComponents() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

protected:
	/*
	* Attach the weapon to the player hand, and snap it to the weapon socket.
	* @param Character : The ATpsCharacterBase on which we want to attach the weapon.
	*/
	UFUNCTION()
		void AttachToPawnHand(ATpsCharacterBase* Character);

	/*
	* Attach the weapon to the player dedicated holster slot.
	* @param Character : The ATpsCharacterBase on which we want to attach the weapon.
	*/
	void AttachToPawnHoslterSlot(ATpsCharacterBase* Character);

		/*
		* Attach and snap this weapon to a socket.
		* @param MeshToAttachOn : USkeletalMeshComponent where we want to attach the weapon.
		* @param SocketName : Socket FName where the weapon should snap.
		* @return True - The attachment was successful.
		*/
	UFUNCTION()
		bool AttachToSocket(USkeletalMeshComponent* MeshToAttachOn, FName SocketName);

	/*
	* Detach the weapon from it's owner.
	* 
	*/
	UFUNCTION()
		void DetachFromPawn();

public:
	const FName GetMuzzleAttachPoint() const;

	void SetPlayer(class ATpsCharacterBase* _Player);

	/*
	* Set a new state and call the needed methods when switching states.
	*/
	void SetWeaponState(EWeaponState NewState);

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
		ATpsCharacterBase* GetParentCharacter() const { return ParentCharacter; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		USkeletalMeshComponent* GetMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		FVector GetMuzzleWorldLocation() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		FVector GetShotWorldDirection() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Getters")
		EWeaponType GetWeaponType() const { return  WeaponType; }

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
};
