// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAimComponent.generated.h"

class AModularWeapon;
class UCameraComponent;

/*
* Must be instanced on the weapon a player pick up.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4TPS_API UWeaponAimComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	AModularWeapon* Weapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName AimCameraSocketName;

	UCameraComponent* PlayerCamera;

public:	
	// Sets default values for this component's properties
	UWeaponAimComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

public:	
	void StartAiming();
	void StopAiming();
};
