// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Controllers/TpsPlayerController.h"

void ATpsPlayerController::QuitGame()
{
	ConsoleCommand("quit");
}

void ATpsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("QuitGame", IE_Pressed, this, &ATpsPlayerController::QuitGame);
}

void ATpsPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATpsPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
}

void ATpsPlayerController::InitInputSystem()
{
	Super::InitInputSystem();
}

void ATpsPlayerController::BeginDestroy()
{
	Super::BeginDestroy();
}

bool ATpsPlayerController::IsGameMenuVisible() const
{
	return false;
}

void ATpsPlayerController::ShowInGameMenu()
{
}

