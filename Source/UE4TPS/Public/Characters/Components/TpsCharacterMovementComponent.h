// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TpsCharacterMovementComponent.generated.h"

class ATpsCharacterBase;
class AModularWeapon;

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	TESTMOVE_NONE	UMETA(DisplayName = "None"),
};

/*
* Either Travel or Combat mode. Define which animations will be played, affect movements and controls.
*/
UENUM(BlueprintType)
enum class ECharacterGlobalMovementMode : uint8
{
	Combat UMETA(DisplayName = "CombatMode"),
	Travel UMETA(DisplayName = "TravelMode"),
};

UENUM(BlueprintType)
enum class ECharacterStance : uint8
{
	Crouch UMETA(DisplayName = "Crouch"),
	Walk UMETA(DisplayName = "Walk"),
	Jog UMETA(DisplayName = "Jog"),
	Run UMETA(DisplayName = "Run"),
	Jump UMETA(DisplayName = "Jump"),
	Falling UMETA(DisplayName = "Falling"),
	None UMETA(DisplayName = "None")
};

/*
* Datas holder for a Character movement config.
*/
USTRUCT(BlueprintType)
struct FMovementConfig
{
	GENERATED_BODY();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	ECharacterStance Stance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ForwardSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BackwardSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SideSpeed;

	FMovementConfig()
	{
		Stance = ECharacterStance::None;
		ForwardSpeed = 0;
		BackwardSpeed = 0;
		SideSpeed = 0;
	}
};

/*
* Datas holder for a complete Character movement state.
* Allow special movement modes (travel/combat) to have a dedicated directionnal speed.
*/
USTRUCT(BlueprintType)
struct FMovementState
{
	GENERATED_BODY();

	/*
	* Time to wait after ending a sprint to sprint again.
	*/
	UPROPERTY(EditDefaultsOnly)
	float SprintRecoveryTime;

	UPROPERTY(EditDefaultsOnly)
	FMovementConfig WalkConfig;

	UPROPERTY(EditDefaultsOnly)
	FMovementConfig JogConfig;

	UPROPERTY(EditDefaultsOnly)
	FMovementConfig RunConfig;

	UPROPERTY(EditDefaultsOnly)
	FMovementConfig CrouchConfig;

	FMovementState()
	{
		SprintRecoveryTime = 0.5f;
		WalkConfig = FMovementConfig();
		JogConfig = FMovementConfig();
		RunConfig = FMovementConfig();
		CrouchConfig = FMovementConfig();
	}
};


/**
 *
 */
UCLASS()
class UE4TPS_API UTpsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FOnSprintStart)
	FOnSprintStart OnSprintStart;

	DECLARE_MULTICAST_DELEGATE(FOnSprintStop)
	FOnSprintStop OnSprintStop;

protected:
	ATpsCharacterBase* TpsCharacter = nullptr;

	/*
	* Combat or Non-combat movements.
	*/
	UPROPERTY(VisibleAnywhere)
	ECharacterGlobalMovementMode MovementType;

	/*
	* Pointer to the currently loaded Movement config.
	*/
	FMovementConfig* ActiveConfig;

	/*
	* Stance determined from the Inputs.
	*/
	ECharacterStance InputCharacterStance;

	bool bWeaponIsInHand = false;

	FVector InputDirection;

	/*
	* Container for the MovementConfigs.
	*/
	UPROPERTY(EditDefaultsOnly)
	FMovementState CharaMovementState;

	/*
	* Delta between last frame Yaw and this frame Yaw from Actor rotation.
	*/
	float FrameYawDelta;

	FRotator LastRotation;

	/*
	* Last time the Character sprinted.
	* Is used to compare with the Sprint recovery time.
	*/
	float SprintLastTime;

	/*
	* True : Movement is independ from the Controller direction.
	* False : Player will alway try to face Controller direction.
	*/
	UPROPERTY(EditAnywhere)
	bool FreeMove = false;

public:
	UTpsCharacterMovementComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE FMovementConfig& GetActiveMovementConfig() const { return *ActiveConfig; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE ECharacterGlobalMovementMode GetMovementype() const { return MovementType; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsRunStance() const { return ActiveConfig->Stance == ECharacterStance::Run; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsJogStance() const { return ActiveConfig->Stance == ECharacterStance::Jog; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsWalkStance() const { return ActiveConfig->Stance == ECharacterStance::Walk; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsAccelerating() const { return Velocity.SizeSquared() > 0.f; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool AllowFiring() const { return ActiveConfig->Stance != ECharacterStance::Run; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsMovingLeft() const { return InputDirection.Y < 0.f; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsMovingRight() const { return InputDirection.Y > 0.f; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool HaveWeaponInHand() const { return bWeaponIsInHand; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetYawDelta() const { return FrameYawDelta; }

	void SetWeaponIsInHand(bool WeaponInHand) { bWeaponIsInHand = WeaponInHand; }

protected:
	/*
	* Switch animations set if a weapon is equipped. Start Combat Mode. Called by Event.
	*/
	UFUNCTION()
	void OnWeaponEquipped(AModularWeapon* NewlyEquippedWeapon);

	/*
	* Switch animations set if a weapon is equipped. Start Travel Mode. Called by Event.
	*/
	UFUNCTION()
	void OnWeaponUnequipped(AModularWeapon* NewlyEquippedWeapon);

	/*
	* Subscribe this object methods to WeaponHandlerComponent Events. Called once in Begin Play.
	*/
	void SubscribeToWeaponHandlerComponent();

public:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Add a Forward/Backward input to the owning pawn, scaled by the actual Movement Type.
	*/
	void MoveForward(float InputValue);

	/*
	* Add a Right/Left input to the owning pawn, scaled by the actual Movement Type.
	*/
	void MoveRight(float InputValue);

	/*
	* Can we Sprint, i.e. enter Run Stance ?
	*/
	bool CanRun();
	
	/*
	* Change movement config. Example : from Jog to Run.
	* @param The new config we want to load.
	* @return The old config.
	*/
	ECharacterStance LoadMovementConfigs(ECharacterStance NewMovementConfig);

	/*
	* Make the Character Jump.
	*/
	virtual void StartJump();
	/*
	* Stop the Jump input.
	*/
	virtual void StopJump();
	/*
	* Called once when the Character touch after a jump.
	*/
	virtual void EndJump();

	void EnableTravelMode();
	void EnableCombatMode();

	void EnableSprint();
	void DisableSprint();

	void EnableWalk();
	void DisableWalk();

};
