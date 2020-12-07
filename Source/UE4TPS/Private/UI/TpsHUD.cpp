// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/TpsHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

#include "Characters/TpsPlayerCharacter.h"
#include "Characters/Components/CharacterWeaponComponent.h"
#include "Weapons/ModularWeapon.h"

ATpsHUD::ATpsHUD()
{
	// Set the crosshair texture
	// static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/Game/Textures/FirstPersonCrosshair"));
	//	CrosshairTex = CrosshairTexObj.Object;
}


void ATpsHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	// const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	// const FVector2D CrosshairDrawPosition(
	//	(Center.X - 8.0f),
	//	(Center.Y + 8.0f)
	// );

	// draw the crosshair
	// FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	// TileItem.BlendMode = SE_BLEND_Translucent;
	// Canvas->DrawItem(TileItem);


}

void ATpsHUD::BeginPlay()
{
	/*
	AFPSCharacter* player = Cast<AFPSCharacter>(GetWorld()->GetFirstPlayerController()->GetOwner());
	if (player)
	{
		AddPostRenderedActor(player);
		AddPostRenderedActor(player->GetWeaponHandlerComponent()->GetEquippedWeapon());
	}
	*/
}
