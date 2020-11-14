// Fill out your copyright notice in the Description page of Project Settings.


#include "UE4TPS/Public/UI/WeaponWidget.h"

#include "Components/TextBlock.h"

#include "Characters/TpsPlayerCharacter.h"
#include "Weapons/WeaponBase.h"

void UWeaponWidget::UpdateWeaponDatas()
{
	if (DisplayedWeapon)
	{
		if (WeaponNameText)
		{
			WeaponNameText->SetText(FText::FromString(DisplayedWeapon->GetName()));
		}
		if (AmmunitionsReserveText)
		{
			AmmunitionsReserveText->SetText(
				FText::FromString(
					FString::FromInt(DisplayedWeapon->GetCurrentAmmunitionsReserve())
					+ FString("/")
					+ FString::FromInt(DisplayedWeapon->GetMaxAmmunitionsReserve())
				)
			);
		}
		if (AmmunitionsInClipText)
		{
			AmmunitionsInClipText->SetText(
				FText::FromString(FString::FromInt(DisplayedWeapon->GetAmmunitionInClip()))
			);
		}
	}
	else
	{
		if (WeaponNameText)
		{
			WeaponNameText->SetText(FText::FromString("No Weapon"));
		}
		if (AmmunitionsReserveText)
		{
			AmmunitionsReserveText->SetText(FText::FromString("0/0"));
		}
		if (AmmunitionsInClipText)
		{
			AmmunitionsInClipText->SetText(FText::FromString("0"));
		}

	}

}

UWeaponWidget::UWeaponWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
void UWeaponWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DisplayedWeapon = Character->GetEquippedWeapon();
	UpdateWeaponDatas();

}

void UWeaponWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	DisplayedWeapon = Character->GetEquippedWeapon();
	UpdateWeaponDatas();
}

void UWeaponWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// When first creating a Blueprint subclass of this class,
	// the widgets won't exist, so we must null check.

}
