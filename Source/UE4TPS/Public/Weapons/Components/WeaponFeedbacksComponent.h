// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponFeedbacksComponent.generated.h"

class AWeaponBase;
class UAnimMontage;
class USoundCue;
class UAudioComponent;
class UParticleSystem;
class UParticleSystemComponent;
class UMaterial;
class UCameraShake;

USTRUCT()
struct FImpactDecalDatas
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
		UMaterial* DecalImpactMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
		float LifeSpan;

	FImpactDecalDatas()
	{
		DecalImpactMaterial = nullptr;
		LifeSpan = 5.0f;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4TPS_API UWeaponFeedbacksComponent : public UActorComponent
{
	GENERATED_BODY()

private:

protected:
	AWeaponBase* OwningWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Particles")
		UParticleSystem* MuzzleFlashFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Particles")
		UParticleSystem* MuzzleHeatFX = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound|Fire")
		USoundCue* FireLoop_Sound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound|Fire")
		USoundCue* FireEnd_Sound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound|Fire")
		USoundCue* FireSingle_Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound|Fire")
		bool bLoopedFireSound = true;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound|Others")
		USoundCue* ReloadFull_Sound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound|Others")
		USoundCue* WeaponRaise_Sound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound|Others")
		USoundCue* WeaponLower_Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animations|Weapon")
		UAnimMontage* WeaponReloadFull_Anim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animations|Player")
		bool bLoopedPlayerFireAnimation = false;

	bool bIsPlayingFireAnimation = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Feedbacks")
		TSubclassOf<UCameraShake> FireCameraShake = NULL;

	UPROPERTY(VisibleAnywhere, Transient, Category = "Weapon|SpawnedComponents")
		UAudioComponent* FireSpawnedAudioComponent = nullptr;

public:
	// Sets default values for this component's properties
	UWeaponFeedbacksComponent();

private:
	UParticleSystemComponent* PlayMuzzleFlashFX();

	UAudioComponent* PlaySound(USoundCue* Sound);
	UAudioComponent* PlayFireSound(bool bIsLoop = false);
	UAudioComponent* PlayFireEndSound();
	UAudioComponent* PlayReloadSound();
	UAudioComponent* PlayRaiseWeaponSound();
	UAudioComponent* PlayLowerWeaponSound();

	void PlayWeaponAnimation(UAnimMontage* Animation);
	void PlayWeaponReloadAnimation();

	void PlayCameraShake();

	void StopFireSound();

	void SpawnImpactDecal(const FHitResult& Impact);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartSimulatingWeaponFiring();
	/*
	* Simulate FX, Sounds, Camera Shake and animations of the weapon when it is firing. Cosmetic Only.
	*/
	void SimulateWeaponFiring();
	/*
	* Fade Out and Stop simulating weapon fire. Cosmetic Only.
	*/
	void StopSimulatingWeaponFiring();

	void SimulateReloading();

	void SimulateRaiseWeapon();
	void SimulateLowerWeapon();

	AWeaponBase* GetOwningWeapon() const;
};
