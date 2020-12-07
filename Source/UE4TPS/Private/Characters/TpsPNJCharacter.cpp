// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TpsPNJCharacter.h"

#include "Characters/Components/CharacterWeaponComponent.h"
#include "Characters/Components/TpsCharacterMovementComponent.h"

ATpsPNJCharacter::ATpsPNJCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	bIsPlayer = false;
}