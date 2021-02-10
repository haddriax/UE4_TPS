// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Interfaces/HitResponsiveInterface.h"

#include "TpsCharacterBase.generated.h"

class UIK_LegsComponent;
// class UWeaponHandlerComponent;
class UCharacterWeaponComponent;
class UTpsCharacterMovementComponent;
class UTpsCharacterStatesComponent;

USTRUCT()
struct FCharacterStats
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere)
		float MaxHealth;

	UPROPERTY(VisibleAnywhere)
		float Health;

	FCharacterStats()
	{
		MaxHealth = 100;
		Health = MaxHealth;
	}

	FCharacterStats(float _MaxHealth)
	{
		MaxHealth = _MaxHealth;
		Health = _MaxHealth;
	}
};

/*
* Base class for human Player and NPC.
*/
UCLASS(Abstract, Blueprintable, BlueprintType)
class UE4TPS_API ATpsCharacterBase : public ACharacter, public IHitResponsiveInterface
{
	GENERATED_BODY()

protected:
	FName WeaponAttachPointOnCharacter_Rifle;
	FName HolsterWeaponAttachPointOnCharacter_Rifle;

	FName WeaponRightHandSocketName;
	FName WeaponLeftHandSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "IK", meta = (AllowPrivateAccess = "true"))
		UIK_LegsComponent* IK_LegsComponent;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
		UWeaponHandlerComponent* WeaponHandlerComponent;
	*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
		UCharacterWeaponComponent* WeaponHandlerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
		UTpsCharacterMovementComponent* TpsCharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
		FCharacterStats Stats;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		UAnimMontage* AM_HitFront;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		UAnimMontage* AM_DieFront;

	/*
	* Pitch applied for the Additive Aim Space.
	* [-90; 90]
	*/
	float AimPitch;

	/*
	* Yaw applied for the Additive Aim Space.
	* [-90; 90]
	*/
	float AimYaw;

	FRotator PrevRotation;

	/*
	* Rotation that occured since last frame.
	*/
	FRotator DeltaRotator;

	uint8 bIsPlayer;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UCharacterWeaponComponent* GetWeaponHandlerComponent() const { return WeaponHandlerComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetWeaponInHandAttachPointOnCharacter() const { return WeaponAttachPointOnCharacter_Rifle; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FName GetHolsterWeaponAttachPointOnCharacter() const { return HolsterWeaponAttachPointOnCharacter_Rifle; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UTpsCharacterMovementComponent* GetTpsCharacterMovementComponent() const { return TpsCharacterMovementComponent; }

	/*
	* Pitch applied for the Additive Aim Space.
	* [-90; 90]
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetAimPitch() const { return AimPitch; }


	/*
	* Yaw applied for the Additive Aim Space.
	* [-90; 90]
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetAimYaw() const { return AimYaw; }

	/*
	* Fast accessor for the equipped weapon.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
		AModularWeapon* GetEquippedWeapon() const;

	// Sets default values for this character's properties
	ATpsCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	void StopAnimMontage(class UAnimMontage* AnimMontage) override;

	/** Begin implementation : HitResponsive Interface */
	void OnHit(const AModularWeapon* HitInstigator) override;
	/* End HitResponsive Interface */


	UFUNCTION(BlueprintCallable)
		void StartFire();
	UFUNCTION(BlueprintCallable)
		void StopFire();

	void ReloadWeapon();

	void AimAtLocation(const FVector& Location);
	void AimAtActor(const AActor* TargetedActor);

	void MoveToLocationWithPath(const FVector& Location);
	void MoveToActorWithPath(const AActor* TargetedActor);

	void EnterCombatMode();
	void EnterRelaxMode();

	void Die();

	void EnableRagdoll();

};