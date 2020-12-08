// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponFireModeComponent.generated.h"

class AModularWeapon;
class ABasicProjectile;

UENUM(BlueprintType)
enum class EStopFireReason : uint8
{
	InputStopped UMETA(DisplayName = "InputStopped"),
	OutOfAmmunitions UMETA(DisplayName = "OutOfAmmunitions"),
};

/*
* Handle behavior for the weapon shooting.
* Subscribe the FOnBurstStarted, FOnBurstEnded, FOnShotwith events (BP || Cpp) with the owning weapon to specify behaviors.
* Animation/FX/Sound done with Blueprint code.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4TPS_API UWeaponFireModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FOnFireStop);
	FOnFireStop OnFireStop;

	DECLARE_MULTICAST_DELEGATE(FOnFireStart);
	FOnFireStart OnFireStart;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBurstStarted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBurstEnded);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShot);

	UPROPERTY(BlueprintAssignable, Category = "FireModeEvents")
		FOnBurstStarted OnBurstStarted;

	UPROPERTY(BlueprintAssignable, Category = "FireModeEvents")
		FOnBurstEnded OnBurstEnded;

	UPROPERTY(BlueprintAssignable, Category = "FireModeEvents")
		FOnShot OnShot;

protected:
	UPROPERTY(BlueprintReadOnly)
		AModularWeapon* Weapon;

	/*
	* If referenced, will spawn the projectile each time the fire shot.
	* The projectile is instanced on the Muzzle socket from the weapon.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<ABasicProjectile> Projectile;

	/*
	* Game Time when the last bullet was fired. Used for the Fire timer.
	*/
	float LastFireTime;

	/*
	* Amount of time shot was called this burst (while Fire is maintained).
	*/
	int32 BurstCounter;

	bool bTryFire;

public:
	// Sets default values for this component's properties
	UWeaponFireModeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	/*
	* Logic called each time the weapon shoot.
	* Must be overriden.
	*/
	virtual void BurstStarted();
	/*
	* Logic called each time the weapon shoot.
	* Must be overriden.
	*/
	virtual void BurstEnded();

	/*
	* Logic called each time the weapon shoot.
	* Must be overriden.
	*/
	virtual void Shot();

	/*
	* What the shot actually do in game world.
	* Must be overriden.
	*/
	virtual void ShotImpl();


	/*
	* True - Weapon will refire, i.e. refire timer is active.
	* False - Burst will be ended after the shot.
	*/
	virtual bool IsRefiring();

	/*
	* True - Weapon can shot a bullet.
	* False - Shot will be canceled.
	*/
	virtual bool CanFire();

	/*
	* Blueprint code called in shot.
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void Shot_Blueprint();

	/*
	 Blueprint code called in shot.
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void BurstStart_Blueprint();

	/*
	* Blueprint code called in shot.
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void BurstEnd_Blueprint();

public:
	/*
	* Shoot with this FireMode.
	* Called externally by the AWeaponBase owner.
	*/
	void BeginFire();

	/*
	* Stop shooting with this FireMode.
	* Called externally by the AWeaponBase owner.
	*/
	void EndFire();
};
