// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "TpsCharacter.generated.h"

UCLASS()
class UE4TPS_API ATpsCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	FName WeaponAttachPointOnCharacter_Rifle;
	FName HolsterWeaponAttachPointOnCharacter_Rifle;

	FName WeaponRightHandSocketName;
	FName WeaponLeftHandSocketName;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "IK", meta = (AllowPrivateAccess = "true"))
		class UIK_LegsComponent* IK_LegsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
		class UWeaponHandlerComponent* WeaponHandlerComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
		class UTpsCharacterMovementComponent* TpsCharacterMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
		class UTpsCharacterStatesComponent* TpsCharacterStatesComponent = nullptr;

	UPROPERTY()
		class UWeaponWidget* WeaponUI = nullptr;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseLookUpRate;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UCameraComponent* GetThirdPersonCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UWeaponHandlerComponent* GetWeaponHandlerComponent() const { return WeaponHandlerComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetWeaponInHandAttachPointOnCharacter() const { return WeaponAttachPointOnCharacter_Rifle; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetHolsterWeaponAttachPointOnCharacter() const { return HolsterWeaponAttachPointOnCharacter_Rifle; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UTpsCharacterStatesComponent* GetTpsCharacterStatesComponent() const { return TpsCharacterStatesComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UTpsCharacterMovementComponent* GetTpsCharacterMovementComponent() const { return TpsCharacterMovementComponent; }

	/*
	* Fast accessor for the equipped weapon.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		AWeaponBase* GetEquippedWeapon() const;

	// Sets default values for this character's properties
	ATpsCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;

	/*
	* Lock/Unlock the camera to the Character orientation.
	*/
	void ToggleLockCamera();

	void StartFire();
	void EndFire();

	void ReloadWeapon();

	void OnReload();
};
