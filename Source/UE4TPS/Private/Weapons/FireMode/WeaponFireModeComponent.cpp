// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FireMode/WeaponFireModeComponent.h"

#include "Weapons/ModularWeapon.h"
#include "Weapons/Projectiles/BasicProjectile.h"
#include "Components/SphereComponent.h"

#include "Characters/TpsCharacterBase.h"

// Sets default values for this component's properties
UWeaponFireModeComponent::UWeaponFireModeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Weapon = nullptr;

	LastFireTime = 0.0f;
	bTryFire = false;
	BurstCounter = 0;
}


// Called when the game starts
void UWeaponFireModeComponent::BeginPlay()
{
	Super::BeginPlay();

	Weapon = Cast<AModularWeapon>(GetOwner());
	check(Weapon);
}

void UWeaponFireModeComponent::InitializeComponent()
{
	Super::InitializeComponent();

	
}

void UWeaponFireModeComponent::BurstStarted()
{
	OnBurstStarted.Broadcast();
}

void UWeaponFireModeComponent::BurstEnded()
{
	BurstCounter = 0;
	OnBurstEnded.Broadcast();
}

void UWeaponFireModeComponent::Shot()
{
	BurstCounter++;
	OnShot.Broadcast();
	
	if (Projectile.Get())
	{
		FActorSpawnParameters asp;
		asp.Instigator = CastChecked<APawn>(Weapon->GetParentCharacter());
		asp.Owner = Weapon;
		asp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABasicProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABasicProjectile>(
			Projectile.Get(),
			Weapon->GetMesh()->GetSocketTransform(Weapon->GetMuzzleFX_SocketName(), ERelativeTransformSpace::RTS_World),
			asp
			);

		SpawnedProjectile->GetSphereCollision()->IgnoreActorWhenMoving(Weapon, true);
		SpawnedProjectile->GetSphereCollision()->IgnoreActorWhenMoving(Weapon->GetParentActor(), true);
	}
}

bool UWeaponFireModeComponent::IsRefiring()
{
	return bTryFire;
}

bool UWeaponFireModeComponent::CanFire()
{
	return true;
}

void UWeaponFireModeComponent::BeginFire()
{
	bTryFire = true;
	if (CanFire())
	{
		BurstStarted();
	}
}

void UWeaponFireModeComponent::EndFire()
{
	bTryFire = false;
	if (BurstCounter > 0)
	{
		BurstEnded();
	}
}
