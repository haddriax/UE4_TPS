// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/WeaponAimComponent.h"

#include "Weapons/ModularWeapon.h"

// Sets default values for this component's properties
UWeaponAimComponent::UWeaponAimComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


// Called when the game starts
void UWeaponAimComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UWeaponAimComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Weapon = Cast<AModularWeapon>(GetOwner());
	check(Weapon);

	check(Weapon->GetMesh()->DoesSocketExist(AimCameraSocketName));

	const USkeletalMeshSocket* Socket =  Weapon->GetMesh()->GetSocketByName(AimCameraSocketName);	
}