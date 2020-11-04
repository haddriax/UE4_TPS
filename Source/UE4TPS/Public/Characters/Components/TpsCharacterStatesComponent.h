// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TpsCharacterStatesComponent.generated.h"


class ATpsCharacter;

USTRUCT(BlueprintType)
struct FCharacterState
{
	GENERATED_BODY();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bIsInTransition = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bIsFiring = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bIsStanding = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bWantsToAim = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bWantsToFire = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bWantsToSprint = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bWantsToCrouch = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bWantsToStand = false;

	FCharacterState()
	{
		bIsInTransition = false;
		bIsAiming = false;
		bIsSprinting = false;

		bIsStanding = true;
		bWantsToStand = true;
	}
};

/*
* Keep trace of Character states, handle States transitions and easy access to datas for the AnimationBlueprint.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4TPS_API UTpsCharacterStatesComponent : public UActorComponent
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(VisibleAnywhere)
		ATpsCharacter* Character = nullptr;

	FCharacterState CharacterState;

	FTimerHandle TimerHandle_Transition;

public:
	// Sets default values for this component's properties
	UTpsCharacterStatesComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool WantsToAim() const { return CharacterState.bWantsToAim; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool WantsToSprint() const { return CharacterState.bWantsToSprint; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool WantsToFire() const { return CharacterState.bWantsToFire; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsAiming() const { return CharacterState.bIsAiming; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsFiring() const { return CharacterState.bIsFiring; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsSprinting() const { return CharacterState.bIsSprinting; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsInTransition() const { return CharacterState.bIsInTransition; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FCharacterState const GetCharacterState() const { return CharacterState; };

	void SetWantsToAim(bool Value);
	void SetWantsToSprint(bool Value);
	void SetWantsToFire(bool Value);

	void TryJump(bool Value);
	void TryReload(bool Value);

	bool CanAimInCurentState() const;
	bool CanFireInCurentState() const;
	bool CanSprintInCurentState() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void EnterSprint();
	void LeaveSprint();

	void EnterJump();
	void LeaveJump();

	void EnterAim();
	void LeaveAim();

	void EnterFire();
	void LeaveFire();

	void EnterReload();
	void LeaveReload();

	void UpdateState();

	/*
	* Wait the specified duration before calling the OnTransitionEnd callback. Reset bIsInTransition to false.
	*/
	void StartTransitionForDuration(float Duration);

	UFUNCTION()
		void TransitionEnd();

	/*
	* This method subscribe to the OnStopFire event of the equipped weapon.
	*/
	void NotifyStopFiring();

public:
	/*
	* Run whenever the Component is spawned or when a property changes.
	*/
	virtual void OnComponentCreated() override;

	virtual void InitializeComponent() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TryAiming();
	void StopTryingAiming();
};
