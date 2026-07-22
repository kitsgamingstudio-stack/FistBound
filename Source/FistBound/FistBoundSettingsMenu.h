// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "FistBoundSettingsMenu.generated.h"

class UFistBoundSaveSubsystem;

/**
 * Base settings screen for the foundation menu flow. Author it in the editor as
 * WBP_SettingsMenu (subclass this) and drive the widgets from the save
 * subsystem: read UFistBoundSaveSubsystem::GetSettings() to initialise controls,
 * and call SetMasterVolume / SetGraphicsQuality (live apply) plus SaveSettings on
 * back. Pushed onto the menu stack by AFistBoundPlayerController::ShowSettings.
 */
UCLASS(Abstract, Blueprintable)
class FISTBOUND_API UFistBoundSettingsMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	/** Convenience accessor for the foundation settings/save subsystem. */
	UFUNCTION(BlueprintPure, Category = "FistBound|Settings")
	UFistBoundSaveSubsystem* GetSaveSubsystem() const;
};
