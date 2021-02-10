// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectiles/BasicProjectile.h" 	

#include "GameFramework/ProjectileMovementComponent.h" 	
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABasicProjectile::ABasicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetRelativeLocation(FVector::ZeroVector);
	SetRootComponent(SphereCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(GetRootComponent());
	ParticleSystem->SetRelativeLocation(FVector::ZeroVector);

	SphereCollision->InitSphereRadius(15.0f);

	ProjectileMovement->SetUpdatedComponent(SphereCollision);
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->MaxSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.2f;
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = 4.0f;

	// Collision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

}

// Called when the game starts or when spawned
void ABasicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
