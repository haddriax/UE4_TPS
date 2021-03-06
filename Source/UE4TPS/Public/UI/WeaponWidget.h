// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PlayerBaseWidget.h"
#include "WeaponWidget.generated.h"

class AModularWeapon;

class UTextBlock;
class UImage;

/**
 *
 */
UCLASS(Blueprintable)
class UE4TPS_API UWeaponWidget : public UPlayerBaseWidget
{
	GENERATED_BODY()

protected:
	AModularWeapon* DisplayedWeapon = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* WeaponNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* AmmunitionsReserveText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* AmmunitionsInClipText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
		UImage* OptionalImage;

protected:
	/*
	* Update every fields related to the weapon.
	*/
	void UpdateWeaponDatas();

public:
	UWeaponWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void SynchronizeProperties() override;
};
