// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerBaseWidget.generated.h"

class ATpsPlayerCharacter;

/**
 *
 */
UCLASS()
class UE4TPS_API UPlayerBaseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	ATpsPlayerCharacter* Character = nullptr;

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void SynchronizeProperties() override;
};
