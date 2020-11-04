// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/Components/IK_LegsComponent.h"
#include "Characters/Components/WeaponHandlerComponent.h"
#include "Characters/Components/TpsCharacterMovementComponent.h"
#include "Characters/Components/TpsCharacterStatesComponent.h"
#include "Weapons/WeaponBase.h"
#include "UI/WeaponWidget.h"

AWeaponBase* ATpsCharacter::GetEquippedWeapon() const
{
	return GetWeaponHandlerComponent()->GetEquippedWeapon();
}

// Sets default values
ATpsCharacter::ATpsCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	WeaponHandlerComponent = CreateDefaultSubobject<UWeaponHandlerComponent>(TEXT("WeaponHandlerComponent"));
	TpsCharacterStatesComponent = CreateDefaultSubobject<UTpsCharacterStatesComponent>(TEXT("TpsCharacterStatesComponent"));

	IK_LegsComponent = CreateDefaultSubobject<UIK_LegsComponent>(TEXT("IK_LegsComponent"));

	WeaponAttachPointOnCharacter_Rifle = FName("middle_01_r_WeaponSocket");
	HolsterWeaponAttachPointOnCharacter_Rifle = FName("RifleHolsterSocket");
}

// Called when the game starts or when spawned
void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Cast<APlayerController>(GetController())->PlayerCameraManager;
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Create and add the weapon widget.
	WeaponUI = CreateWidget<UWeaponWidget>(PC, UWeaponWidget::StaticClass()); 
	
	FInputModeGameOnly Mode;
	Mode.SetConsumeCaptureMouseDown(false);
	PC->SetInputMode(Mode); 
	WeaponUI->AddToViewport(9999);

	// Subscibe to Sprint Start, so sprinting instantly stop the character from shooting.
	GetTpsCharacterMovementComponent()->OnSprintStart.AddUObject(this, &ATpsCharacter::EndFire);
}

void ATpsCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TpsCharacterMovementComponent = Cast<UTpsCharacterMovementComponent>(GetCharacterMovement());

	check(TpsCharacterMovementComponent);
	check(TpsCharacterStatesComponent);
	check(IK_LegsComponent);
	check(WeaponHandlerComponent);
	check(CameraBoom);

	/*
	if (RespawnFX)
	{
		UGameplayStatics::SpawnEmitterAttached(RespawnFX, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
	}
	*/
}

// Called every frame
void ATpsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::EnableSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::DisableSprint);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::EnableWalk);
	PlayerInputComponent->BindAction("Walk", IE_Released, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::DisableWalk);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATpsCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATpsCharacter::EndFire);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, TpsCharacterStatesComponent, &UTpsCharacterStatesComponent::TryAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, TpsCharacterStatesComponent, &UTpsCharacterStatesComponent::StopTryingAiming);

	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::EquipPrimaryWeapon);
	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::Reload);

	// PlayerInputComponent->BindAction("CombatMode", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::ManageWeaponHolding);

	PlayerInputComponent->BindAction("LockCamera", IE_Pressed, this, &ATpsCharacter::ToggleLockCamera);

	PlayerInputComponent->BindAxis("MoveForward", TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATpsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATpsCharacter::LookUpAtRate);
}

float ATpsCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate /* = 1.0f */, FName StartSectionName /* = FNAME_NONE */)
{
	USkeletalMeshComponent* UseMesh = GetMesh();

	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}

	return 0.0f;
}

void ATpsCharacter::StopAnimMontage(UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetMesh();

	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance &&
		UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage) == true)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}
}

void ATpsCharacter::ToggleLockCamera()
{
	TpsCharacterMovementComponent->bOrientRotationToMovement = !TpsCharacterMovementComponent->bOrientRotationToMovement;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
}

void ATpsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATpsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATpsCharacter::StartFire()
{
	AWeaponBase* const weapon = GetWeaponHandlerComponent()->GetEquippedWeapon();

	if (weapon 
		&& GetTpsCharacterStatesComponent()->CanFireInCurentState() == true
		&& GetTpsCharacterMovementComponent()->IsSprinting() == false)
	{
		TpsCharacterStatesComponent->SetWantsToFire(true);
	}
}

void ATpsCharacter::EndFire()
{
	AWeaponBase* const weapon = GetWeaponHandlerComponent()->GetEquippedWeapon();

	if (weapon && GetTpsCharacterStatesComponent()->IsFiring() == true)
	{
		TpsCharacterStatesComponent->SetWantsToFire(false);
	}
}

void ATpsCharacter::ReloadWeapon()
{
	GetWeaponHandlerComponent()->Reload();
}

void ATpsCharacter::OnReload()
{
}
