// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/TpsCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/Components/IK_LegsComponent.h"
#include "Characters/Components/WeaponHandlerComponent.h"
#include "Characters/Components/TpsCharacterMovementComponent.h"
#include "Characters/Components/TpsCharacterStatesComponent.h"
#include "Weapons/WeaponBase.h"


AWeaponBase* ATpsCharacterBase::GetEquippedWeapon() const
{
	return GetWeaponHandlerComponent()->GetEquippedWeapon();
}

// Sets default values
ATpsCharacterBase::ATpsCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	WeaponHandlerComponent = CreateDefaultSubobject<UWeaponHandlerComponent>(TEXT("WeaponHandlerComponent"));
	TpsCharacterStatesComponent = CreateDefaultSubobject<UTpsCharacterStatesComponent>(TEXT("TpsCharacterStatesComponent"));

	IK_LegsComponent = CreateDefaultSubobject<UIK_LegsComponent>(TEXT("IK_LegsComponent"));

	WeaponAttachPointOnCharacter_Rifle = FName("middle_01_r_WeaponSocket");
	HolsterWeaponAttachPointOnCharacter_Rifle = FName("RifleHolsterSocket");
}

// Called when the game starts or when spawned
void ATpsCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Subscibe to Sprint Start, so sprinting instantly stop the character from shooting.
	GetTpsCharacterMovementComponent()->OnSprintStart.AddUObject(this, &ATpsCharacterBase::StopFire);
}

void ATpsCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TpsCharacterMovementComponent = Cast<UTpsCharacterMovementComponent>(GetCharacterMovement());

	check(TpsCharacterMovementComponent);
	check(TpsCharacterStatesComponent);
	check(IK_LegsComponent);
	check(WeaponHandlerComponent);
}

// Called every frame
void ATpsCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATpsCharacterBase::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	USkeletalMeshComponent* UseMesh = GetMesh();

	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}

	return 0.0f;
}

void ATpsCharacterBase::StopAnimMontage(UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetMesh();

	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance &&
		UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}
}

void ATpsCharacterBase::OnHit(const AWeaponBase* HitInstigator)
{
	check(HitInstigator)

	UE_LOG(LogTemp, Display, TEXT("V %s - Got hit by %s"), TEXT(__FUNCTION__), *HitInstigator->GetHumanReadableName());
}


void ATpsCharacterBase::StartFire()
{
	AWeaponBase* weapon = GetWeaponHandlerComponent()->GetEquippedWeapon();

	if (weapon &&
		GetTpsCharacterStatesComponent()->CanFireInCurentState()
		&& GetTpsCharacterMovementComponent()->AllowFiring())
	{
		TpsCharacterStatesComponent->SetWantsToFire(true);
	}
}

void ATpsCharacterBase::StopFire()
{
	AWeaponBase* weapon = GetWeaponHandlerComponent()->GetEquippedWeapon();

	if (weapon && GetTpsCharacterStatesComponent()->IsFiring() == true)
	{
		TpsCharacterStatesComponent->SetWantsToFire(false);
	}
}

void ATpsCharacterBase::ReloadWeapon()
{
	GetWeaponHandlerComponent()->Reload();
}

void ATpsCharacterBase::AimAtLocation(const FVector& Location)
{
}

void ATpsCharacterBase::AimAtActor(const AActor* TargetedActor)
{
}

void ATpsCharacterBase::MoveToLocationWithPath(const FVector& Location)
{
}

void ATpsCharacterBase::MoveToActorWithPath(const AActor* TargetedActor)
{
}

void ATpsCharacterBase::EnterCombatMode()
{
}

void ATpsCharacterBase::EnterRelaxMode()
{
}
