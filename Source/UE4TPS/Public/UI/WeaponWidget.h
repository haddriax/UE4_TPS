// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWidget.generated.h"

/**
 *
 */
UCLASS(Blueprintable, Blueprintable)
class UE4TPS_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	class ATpsCharacter* Character = nullptr;
	class AWeaponBase* DisplayedWeapon = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* WeaponNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* AmmunitionsReserveText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* AmmunitionsInClipText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
		class UImage* OptionalImage;

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
