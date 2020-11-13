// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TpsPlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"

#include "Characters/Components/WeaponHandlerComponent.h"
#include "Characters/Components/TpsCharacterMovementComponent.h"
#include "Characters/Components/TpsCharacterStatesComponent.h"
#include "UI/WeaponWidget.h"


ATpsPlayerCharacter::ATpsPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ATpsPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());

	check(PC);

	// Create and add the weapon widget.
	WeaponUI = CreateWidget<UWeaponWidget>(PC, UWeaponWidget::StaticClass());
	
	if (WeaponUI)
	{
		FInputModeGameOnly Mode;
		Mode.SetConsumeCaptureMouseDown(false);
		PC->SetInputMode(Mode);
		WeaponUI->AddToViewport(9999);
	}
}

void ATpsPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATpsPlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATpsPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATpsPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATpsPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::EnableSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::DisableSprint);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::EnableWalk);
	PlayerInputComponent->BindAction("Walk", IE_Released, TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::DisableWalk);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATpsPlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATpsPlayerCharacter::StopFire);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, TpsCharacterStatesComponent, &UTpsCharacterStatesComponent::TryAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, TpsCharacterStatesComponent, &UTpsCharacterStatesComponent::StopTryingAiming);

	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::EquipPrimaryWeapon);
	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::Reload);

	// PlayerInputComponent->BindAction("CombatMode", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::ManageWeaponHolding);

	PlayerInputComponent->BindAction("LockCamera", IE_Pressed, this, &ATpsPlayerCharacter::ToggleLockCamera);

	PlayerInputComponent->BindAxis("MoveForward", TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATpsPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATpsPlayerCharacter::LookUpAtRate);
}

void ATpsPlayerCharacter::ToggleLockCamera()
{
	TpsCharacterMovementComponent->bOrientRotationToMovement = !TpsCharacterMovementComponent->bOrientRotationToMovement;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
}
