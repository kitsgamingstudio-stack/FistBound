// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FistBoundGameInstance.generated.h"

class SWidget;
class UUserWidget;

/**
 * Genre-neutral GameInstance entry point for FistBound-derived games.
 * Owns the foundation subsystems (UI, Save) for their whole lifetime, and drives
 * the foundation loading screen during level transitions via the engine
 * MoviePlayer. Derived games subclass this rather than editing it.
 */
UCLASS(Config = Game)
class FISTBOUND_API UFistBoundGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

protected:
	/** Shows the foundation loading screen during every level transition. */
	UPROPERTY(EditDefaultsOnly, Config, Category = "FistBound|Loading")
	bool bLoadingScreenEnabled = true;

	/**
	 * UMG widget shown while the next level streams in. Leave unset to use the
	 * built-in native Slate loading screen. Set it on a Blueprint subclass of
	 * this GameInstance (or via config) to reskin the loading screen per game.
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "FistBound|Loading")
	TSoftClassPtr<UUserWidget> LoadingScreenWidgetClass;

	/** Floor on how long the loading screen stays up, in seconds; 0 = no floor. */
	UPROPERTY(EditDefaultsOnly, Config, Category = "FistBound|Loading", meta = (ClampMin = "0.0"))
	float MinimumLoadingScreenDisplaySeconds = 0.0f;

private:
	/** Arms the MoviePlayer loading screen before the next map load. */
	void HandlePreLoadMap(const FString& MapName);

	/** Releases the loading widget once the new world is ready. */
	void HandlePostLoadMap(UWorld* LoadedWorld);

	/** Builds the configured UMG widget, or the native Slate fallback. */
	TSharedRef<SWidget> BuildLoadingScreenWidget();

	/** Keeps the created UMG widget alive while the loading screen is shown. */
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> ActiveLoadingWidget = nullptr;
};
