// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicProjectile.generated.h"

class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS(BlueprintType, Blueprintable)
class UE4TPS_API ABasicProjectile : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* ParticleSystem;

public:
	// Sets default values for this actor's properties
	ABasicProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
