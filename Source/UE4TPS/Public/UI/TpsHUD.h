#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TpsHUD.generated.h"

UCLASS()
class UE4TPS_API ATpsHUD : public AHUD
{
	GENERATED_BODY()


public:
	ATpsHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	void DrawCrosshair();
	void DrawWeaponDatas();

protected:
	void BeginPlay() override;
};
