// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponFireModeComponent.generated.h"

class AModularWeapon;

UENUM(BlueprintType)
enum class EStopFireReason : uint8
{
	InputStopped UMETA(DisplayName = "InputStopped"),
	OutOfAmmunitions UMETA(DisplayName = "OutOfAmmunitions"),
};

/*
* * Handle behavior for the weapon shooting.
*/
UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4TPS_API UWeaponFireModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FOnFireStop);
	FOnFireStop OnFireStop;

	DECLARE_MULTICAST_DELEGATE(FOnFireStart);
	FOnFireStart OnFireStart;

	DECLARE_MULTICAST_DELEGATE(FOnShot);
	FOnShot OnShot;
	 
protected:
	UPROPERTY(BlueprintReadOnly)
		AModularWeapon* Weapon;

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
