// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TpsCharacterMovementComponent.generated.h"

class ATpsCharacterBase;
class AWeaponBase;

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
enum ECharacterStance
{
	Crouch,
	Walk,
	Jog,
	Run,
	None
};

USTRUCT()
struct FMovementConfig
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly)
		float ForwardSpeed;
	UPROPERTY(EditDefaultsOnly)
		float BackwardSpeed;
	UPROPERTY(EditDefaultsOnly)
		float SideSpeed;

	/*
	* How much from the player [-1;1] input is actually really used for Forward displacement. Actually changed depending of the Movemement state (ie. walk, jog ...).
	*/
	UPROPERTY(EditDefaultsOnly)
		float ForwardMovementResponsivity = 1.0f;
	/*
	* How much from the player [-1;1] input is actually really used for Right displacement. Actually changed depending of the Movemement state (ie. walk, jog ...).
	*/
	UPROPERTY(EditDefaultsOnly)
		float RightMovementResponsivity = 1.0f;

	FMovementConfig()
	{

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

	UPROPERTY(VisibleAnywhere)
		ECharacterGlobalMovementMode MovementType;

	UPROPERTY(EditDefaultsOnly)
		FMovementConfig CrouchConfig;
	UPROPERTY(EditDefaultsOnly)
		FMovementConfig WalkConfig;
	UPROPERTY(EditDefaultsOnly)
		FMovementConfig JogConfig;
	UPROPERTY(EditDefaultsOnly)
		FMovementConfig RunConfig;

	FMovementConfig* ActiveConfig;

	ECharacterStance LoadedCharacterStance;
	ECharacterStance InputCharacterStance;

	bool bWeaponIsInHand = false;

	UPROPERTY(EditAnywhere)
		float WalkSpeed = 128.195f;
	UPROPERTY(EditAnywhere)
		float JogSpeed = 346.600f;
	UPROPERTY(EditAnywhere)
		float SprintSpeed = 630.450f;

	/*
	* How much from the player [-1;1] input is actually really used for Forward displacement, when walking.
	*/
	UPROPERTY(EditAnywhere)
		float WalkForwardMovementResponsivity = 1.0f;
	/*
	* How much from the player [-1;1] input is actually really used for Forward displacement, when Jogging.
	*/
	UPROPERTY(EditAnywhere)
		float JogForwardMovementResponsivity = 1.0f;
	/*
	* How much from the player [-1;1] input is actually really used for Forward displacement, when Sprinting.
	*/
	UPROPERTY(EditAnywhere)
		float SprintForwardMovementResponsivity = 1.0f;

	/*
	* How much from the player [-1;1] input is actually really used for Right displacement, when Walking.
	*/
	UPROPERTY(EditAnywhere)
		float WalkRightMovementResponsivity = 1.0f;
	/*
	* How much from the player [-1;1] input is actually really used for Right displacement, when Jogging.
	*/
	UPROPERTY(EditAnywhere)
		float JogRightMovementResponsivity = 0.8f;
	/*
	* How much from the player [-1;1] input is actually really used for Right displacement, when Sprinting.
	*/
	UPROPERTY(EditAnywhere)
		float SprintRightMovementResponsivity = 0.35f;

	/*
	* How much from the player [-1;1] input is actually really used for Forward displacement. Actually changed depending of the Movemement state (ie. walk, jog ...).
	*/
	float ForwardMovementResponsivity = 1.0f;
	/*
	* How much from the player [-1;1] input is actually really used for Right displacement. Actually changed depending of the Movemement state (ie. walk, jog ...).
	*/
	float RightMovementResponsivity = 1.0f;

public:
	UTpsCharacterMovementComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE ECharacterGlobalMovementMode GetMovementype() const { return MovementType; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool IsSprinting() const { return LoadedCharacterStance == ECharacterStance::Run; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool HaveWeaponInHand() const { return bWeaponIsInHand; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetForwardMovementResponsivity() const { return ForwardMovementResponsivity; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetRightMovementResponsivity() const { return RightMovementResponsivity; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool AllowFiring() const { return LoadedCharacterStance != ECharacterStance::Run; }

	void SetWeaponIsInHand(bool WeaponInHand) { bWeaponIsInHand = WeaponInHand; }

protected:
	/*
	* Switch animations set if a weapon is equipped. Start Combat Mode. Called by Event.
	*/
	UFUNCTION()
		void OnWeaponEquipped(AWeaponBase* NewlyEquippedWeapon);

	/*
	* Switch animations set if a weapon is equipped. Start Travel Mode. Called by Event.
	*/
	UFUNCTION()
		void OnWeaponUnequipped(AWeaponBase* NewlyEquippedWeapon);

	/*
	* Subscribe this object methods to WeaponHandlerComponent Events. Called once in Begin Play.
	*/
	void SubscribeToWeaponHandlerComponent();

	/*
	* Apply the config.
	*/
	void ApplyMovementConfigs(FMovementConfig const& MovementConfigs);

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
	* Change movement config. Example : from Jog to Run.
	* @param The new config we want to load.
	* @return The old config.
	*/
	ECharacterStance LoadMovementConfigs(ECharacterStance NewMovementConfig);

	void EnableTravelMode();
	void EnableCombatMode();

	void EnableSprint();
	void DisableSprint();

	void EnableWalk();
	void DisableWalk();

};
