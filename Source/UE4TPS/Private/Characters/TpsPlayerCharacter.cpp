// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TpsPlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

#include "Characters/Components/WeaponHandlerComponent.h"
#include "Characters/Components/TpsCharacterMovementComponent.h"
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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoPossessAI = EAutoPossessAI::Disabled;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bIgnoreBaseRotation = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ATpsPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());

	check(PC);

	// Create and add the weapon widget.
	WeaponUI = CreateWidget<UWeaponWidget>(PC, WeaponUIClass);
	if (WeaponUI)
	{
		FInputModeGameOnly Mode;
		Mode.SetConsumeCaptureMouseDown(false);
		PC->SetInputMode(Mode);
		WeaponUI->AddToViewport(32);
	}

	// CastChecked<APlayerController>(GetController())->ClientSetCameraFade

	// ToggleLockCamera();
}

void ATpsPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!Cast<UTpsCharacterMovementComponent>(GetCharacterMovement()))
	{
		UE_LOG(LogTemp, Display, TEXT("X % - GetCharacterMovement() can not be cast to UTpsCharacterMovementComponent."), TEXT(__FUNCTION__));
	}
}

void ATpsPlayerCharacter::AddControllerYawInput(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddYawInput(Val);
	}
}

void ATpsPlayerCharacter::AddControllerPitchInput(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddPitchInput(Val);
	}
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

FHitResult ATpsPlayerCharacter::TraceFromCameraCenter()
{
	const UWorld* World = GetWorld();

	FHitResult Hit;

	if (World)
	{
		const FVector TraceStart = FollowCamera->GetComponentLocation();
		const FVector TraceEnd = FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * 1000);
		// GetWorld()->AsyncLineTraceByChannel
		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams::DefaultResponseParam);
	}

	return Hit;
}

void ATpsPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FHitResult CameraTraceHit = TraceFromCameraCenter();

	// Compute Aim Pitch (used in AnimBP).
	const FVector TraceWorldEnd = (CameraTraceHit.bBlockingHit ? CameraTraceHit.ImpactPoint : CameraTraceHit.TraceEnd);
	const FRotator AimRotator = UKismetMathLibrary::FindLookAtRotation(CameraTraceHit.TraceStart, TraceWorldEnd);
	AimPitch = AimRotator.Pitch;

	const FRotator Local_ControlRotation =
		GetRootComponent()->GetComponentTransform().InverseTransformRotation(
			GetController()->GetControlRotation().Quaternion()
		).Rotator();

	// Compute Aim Yaw, which is the delta Yaw between Character Movement Direction and Controller.
	if (GetCharacterMovement()->Velocity.SizeSquared() > 0)
	{
		const FRotator Local_Velocity = FRotationMatrix::MakeFromXZ(
			GetRootComponent()->GetComponentTransform().InverseTransformVector(GetCharacterMovement()->Velocity),
			FVector::UpVector
		).Rotator();

		AimYaw = Local_Velocity.Yaw - Local_ControlRotation.Yaw;
	}
	else
	{
		AimYaw = 0 + Local_ControlRotation.Yaw;
	}

	// GEngine->AddOnScreenDebugMessage(1288, 10.0f, FColor::Emerald, FString::SanitizeFloat(AimYaw));
	
	/*
	TArray<FName> CurvesNames;
	GetMesh()->AnimScriptInstance->GetAllCurveNames(CurvesNames);

	int i = 0;
	for (FName name : CurvesNames)
	{
		// GEngine->AddOnScreenDebugMessage(i++, 0.0f, FColor::Green, name.ToString());
	}

	float Speed;
	if (GetMesh()->AnimScriptInstance->GetCurveValue(FName("Speed"), Speed))
	{
		// GEngine->AddOnScreenDebugMessage(i++, 0.0f, FColor::Green, FString::SanitizeFloat(Speed));
	}

	// FAnimMontageInstance* m;
	//m = GetMesh()->AnimScriptInstance->GetActiveMontageInstance();
	*/
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

	// PlayerInputComponent->BindAction("Aim", IE_Pressed, TpsCharacterStatesComponent, &UTpsCharacterStatesComponent::TryAiming);
	// PlayerInputComponent->BindAction("Aim", IE_Released, TpsCharacterStatesComponent, &UTpsCharacterStatesComponent::StopTryingAiming);

	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::EquipPrimaryWeapon);
	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::Reload);

	// PlayerInputComponent->BindAction("CombatMode", IE_Pressed, WeaponHandlerComponent, &UWeaponHandlerComponent::ManageWeaponHolding);

	PlayerInputComponent->BindAction("LockCamera", IE_Pressed, this, &ATpsPlayerCharacter::ToggleLockCamera);

	PlayerInputComponent->BindAxis("MoveForward", TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", TpsCharacterMovementComponent, &UTpsCharacterMovementComponent::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &ATpsPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATpsPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ATpsPlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATpsPlayerCharacter::LookUpAtRate);
}

void ATpsPlayerCharacter::ToggleLockCamera()
{
	TpsCharacterMovementComponent->bOrientRotationToMovement = !TpsCharacterMovementComponent->bOrientRotationToMovement;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
}
