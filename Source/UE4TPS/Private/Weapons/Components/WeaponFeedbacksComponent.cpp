// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/WeaponFeedbacksComponent.h"

#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#include "Weapons/ModularWeapon.h"
#include "Characters/TpsCharacterBase.h"

// Sets default values for this component's properties
UWeaponFeedbacksComponent::UWeaponFeedbacksComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

UParticleSystemComponent* UWeaponFeedbacksComponent::PlayMuzzleFlashFX()
{
	UParticleSystemComponent* MuzzleParticleSystemComp = nullptr;

	if (MuzzleFlashFX)
	{
		MuzzleParticleSystemComp = UGameplayStatics::SpawnEmitterAttached(
			MuzzleFlashFX,
			OwningWeapon->GetMesh(),
			OwningWeapon->GetMuzzleAttachPoint()
		);
	}

	return MuzzleParticleSystemComp;
}

UAudioComponent* UWeaponFeedbacksComponent::PlaySound(USoundCue* Sound)
{
	if (Sound)
	{
		return UGameplayStatics::SpawnSoundAttached(Sound, OwningWeapon->GetParentCharacter()->GetRootComponent());
	}
	else
	{
		return nullptr;
	}
}

UAudioComponent* UWeaponFeedbacksComponent::PlayFireSound(bool bIsLoop)
{
	if (bIsLoop == true)
	{
		if (!FireSpawnedAudioComponent)
		{
			FireSpawnedAudioComponent = PlaySound(FireLoop_Sound);
		}
	}
	else
	{
		return PlaySound(FireSingle_Sound);
	}

	return FireSpawnedAudioComponent;
}

UAudioComponent* UWeaponFeedbacksComponent::PlayFireEndSound()
{
	return PlaySound(FireEnd_Sound);
}

UAudioComponent* UWeaponFeedbacksComponent::PlayReloadSound()
{
	return PlaySound(ReloadFull_Sound);
}

UAudioComponent* UWeaponFeedbacksComponent::PlayRaiseWeaponSound()
{
	return PlaySound(WeaponRaise_Sound);
}

UAudioComponent* UWeaponFeedbacksComponent::PlayLowerWeaponSound()
{
	return PlaySound(WeaponLower_Sound);
}

void UWeaponFeedbacksComponent::PlayWeaponAnimation(UAnimMontage* Animation)
{
	if (Animation)
	{
		GetOwningWeapon()->GetMesh()->PlayAnimation(Animation, false);
	}
}

void UWeaponFeedbacksComponent::PlayWeaponReloadAnimation()
{
	PlayWeaponAnimation(WeaponReloadFull_Anim);
}

void UWeaponFeedbacksComponent::StopFireSound()
{
	if (FireSpawnedAudioComponent)
	{
		FireSpawnedAudioComponent->FadeOut(0.1f, 0.0f);
		FireSpawnedAudioComponent = nullptr;
	}

	if (FireEnd_Sound)
	{
		PlaySound(FireEnd_Sound);
	}
}

void UWeaponFeedbacksComponent::SpawnImpactDecal(const FHitResult& Impact)
{
	if (Impact.bBlockingHit)
	{
		FTransform decalTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
	}
}

// Called when the game starts
void UWeaponFeedbacksComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningWeapon = Cast<AModularWeapon>(GetOwner());
}


// Called every frame
void UWeaponFeedbacksComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TICK IS DISABLED.
}

void UWeaponFeedbacksComponent::StartSimulatingWeaponFiring()
{

}

void UWeaponFeedbacksComponent::SimulateWeaponFiring()
{
	PlayFireSound(bLoopedFireSound);
	PlayMuzzleFlashFX();
}

void UWeaponFeedbacksComponent::StopSimulatingWeaponFiring()
{
	StopFireSound();
}

void UWeaponFeedbacksComponent::SimulateReloading()
{
	PlayWeaponReloadAnimation();
}

void UWeaponFeedbacksComponent::SimulateRaiseWeapon()
{
	PlayRaiseWeaponSound();
}

void UWeaponFeedbacksComponent::SimulateLowerWeapon()
{
	PlayLowerWeaponSound();
}

AModularWeapon* UWeaponFeedbacksComponent::GetOwningWeapon() const
{
	check(OwningWeapon);

	return OwningWeapon;
}

