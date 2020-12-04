// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/TpsCharacterBase.h"
#include "TpsPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWeaponWidget;

/**
 * Tps character designed to receive input from a Player.
 * Also add and handle camera.
 */
UCLASS()
class UE4TPS_API ATpsPlayerCharacter : public ATpsCharacterBase
{
	GENERATED_BODY()
protected:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UWeaponWidget> WeaponUIClass;

	UWeaponWidget* WeaponUI = nullptr;

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

	ATpsPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void AddControllerYawInput(float Val) override;

	void AddControllerPitchInput(float Val) override;

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

	FHitResult TraceFromCameraCenter();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*
	* Lock/Unlock the camera to the Character orientation.
	*/
	void ToggleLockCamera();

	/** Begin implementation : HitResponsive Interface */
	// void OnHit(const AWeaponBase* HitInstigator) override;
	/* End HitResponsive Interface */
};
