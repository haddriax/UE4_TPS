// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicProjectile.generated.h"

class UProjectileMovementComponent;
class UParticleSystemComponent;
class USphereComponent;

/*
* Base class for any projectile.
* Not abstract, can be used for basic projectile.
*/
UCLASS(BlueprintType, Blueprintable)
class UE4TPS_API ABasicProjectile : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* ParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* SphereCollision;

public:
	// Sets default values for this actor's properties
	ABasicProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FORCEINLINE USphereComponent* GetSphereCollision() { return SphereCollision;  };
	
	// Called every frame
	virtual void Tick(float DeltaTime) override; 

};
