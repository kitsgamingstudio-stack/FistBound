// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FistBoundTrialHUD.generated.h"

class UFont;

/**
 * Canvas-drawn trial presentation: player health, encounter progress, trial
 * timer, boss bar, center banners, and the victory/defeat result screen.
 * Reads state from UFistBoundTrialSubsystem and the health components only;
 * it owns no combat rules. Placeholder presentation until UMG/CommonUI screens
 * are authored under Content/FistBound/.
 */
UCLASS()
class FISTBOUND_API AFistBoundTrialHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawBar(float X, float Y, float Width, float Height, float Fraction, const FLinearColor& FillColor);
	void DrawCenteredText(const UFont* Font, const FString& Text, float CenterX, float Y, const FLinearColor& Color, float Scale);
	static FString FormatTime(float Seconds);
};
