// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FistBoundLoadingScreenWidget.generated.h"

/**
 * Base UMG widget for the foundation loading screen. Author it in the editor as
 * WBP_LoadingScreen (subclass this) under Content/FistBound/UI/Loading and point
 * UFistBoundGameInstance::LoadingScreenWidgetClass at it to reskin per game.
 *
 * The engine MoviePlayer renders this while the next level streams in, so keep
 * it self-contained: it has no gameplay world and its content should not depend
 * on one. When no widget class is configured the GameInstance shows a native
 * Slate fallback instead, so the loading screen works before any WBP exists.
 */
UCLASS(Abstract, Blueprintable)
class FISTBOUND_API UFistBoundLoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()
};
