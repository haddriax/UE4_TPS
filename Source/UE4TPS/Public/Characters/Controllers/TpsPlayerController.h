// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TpsPlayerController.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class UE4TPS_API ATpsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	TSharedPtr<UUserWidget> IngameMenu;

protected:
	void QuitGame();

public:
	virtual void SetupInputComponent() override;
	virtual void PostInitializeComponents() override;
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void InitInputSystem() override;

	virtual void BeginDestroy() override;

	bool IsGameMenuVisible() const;

	void ShowInGameMenu();
};
