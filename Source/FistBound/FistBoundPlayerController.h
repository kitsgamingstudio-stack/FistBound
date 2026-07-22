// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FistBoundPlayerController.generated.h"

class UCommonActivatableWidget;
class UCommonUserWidget;
class UFistBoundUISubsystem;
class UInputMappingContext;
class UInputAction;

/**
 * Foundation PlayerController. Spawns the primary CommonUI layout, drives the
 * genre-neutral menu/pause flow through UFistBoundUISubsystem, and owns the
 * menu<->gameplay level transitions. Screen classes, input assets and level
 * references are set on a Blueprint subclass so no C++ change is needed to
 * reskin the shell or repoint the flow.
 */
UCLASS()
class FISTBOUND_API AFistBoundPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFistBoundPlayerController();

	/** Push the main menu and switch to UI input. Call from the menu map. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void ShowMainMenu();

	/** Push the settings screen onto the menu stack (menu/pause "Settings"). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void ShowSettings();

	/** Pop the top-most menu screen (generic "Back"; leaves pause/input as-is). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void CloseTopMenu();

	/** Flip the paused state (bound to PauseAction; also callable from a Resume button). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void TogglePause();

	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void SetPaused(bool bNewPaused);

	UFUNCTION(BlueprintPure, Category = "FistBound|UI")
	bool IsGamePaused() const { return bIsPaused; }

	/** Travel to the configured gameplay level (menu "Play" button). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Flow")
	void StartGame();

	/** Unpause and travel back to the configured main menu level (pause "Quit to Menu"). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Flow")
	void ReturnToMainMenu();

	/** Push a confirm dialog before quitting (menu "Quit" button). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Flow")
	void ShowConfirmQuit();

	/** Quit the application (confirm dialog "Yes"). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Flow")
	void QuitGame();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/** Root CommonUI layout that hosts the menu stack. */
	UPROPERTY(EditDefaultsOnly, Category = "FistBound|UI")
	TSubclassOf<UCommonUserWidget> PrimaryLayoutClass;

	UPROPERTY(EditDefaultsOnly, Category = "FistBound|UI")
	TSubclassOf<UCommonActivatableWidget> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "FistBound|UI")
	TSubclassOf<UCommonActivatableWidget> PauseMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "FistBound|UI")
	TSubclassOf<UCommonActivatableWidget> SettingsMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "FistBound|UI")
	TSubclassOf<UCommonActivatableWidget> ConfirmQuitClass;

	/** Mapping context added for the local player on possession. */
	UPROPERTY(EditDefaultsOnly, Category = "FistBound|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Input action that toggles pause. */
	UPROPERTY(EditDefaultsOnly, Category = "FistBound|Input")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(EditDefaultsOnly, Category = "FistBound|Flow")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category = "FistBound|Flow")
	TSoftObjectPtr<UWorld> GameplayLevel;

private:
	UFistBoundUISubsystem* GetUISubsystem() const;
	void SetUIInputMode(bool bUIOnly);

	UPROPERTY(Transient)
	TObjectPtr<UCommonUserWidget> PrimaryLayout = nullptr;

	bool bIsPaused = false;
};
