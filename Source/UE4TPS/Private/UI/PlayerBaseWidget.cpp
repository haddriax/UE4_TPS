// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerBaseWidget.h"

#include "Characters/TpsPlayerCharacter.h"

void UPlayerBaseWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UPlayerBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Character = Cast<ATpsPlayerCharacter>(GetOwningPlayerPawn());
	check(Character);
}

void UPlayerBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPlayerBaseWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}
