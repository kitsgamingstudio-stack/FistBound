// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundPlayerController.h"
#include "FistBoundUISubsystem.h"
#include "Blueprint/UserWidget.h"
#include "CommonUserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AFistBoundPlayerController::AFistBoundPlayerController()
{
	bShowMouseCursor = false;
}

void AFistBoundPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && PrimaryLayoutClass && !PrimaryLayout)
	{
		PrimaryLayout = CreateWidget<UCommonUserWidget>(this, PrimaryLayoutClass);
		if (PrimaryLayout)
		{
			PrimaryLayout->AddToViewport();
		}
	}
}

void AFistBoundPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
		{
			EnhancedInput->BindAction(PauseAction, ETriggerEvent::Started, this, &AFistBoundPlayerController::TogglePause);
		}
	}
}

UFistBoundUISubsystem* AFistBoundPlayerController::GetUISubsystem() const
{
	if (const UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<UFistBoundUISubsystem>();
	}
	return nullptr;
}

void AFistBoundPlayerController::SetUIInputMode(bool bUIOnly)
{
	if (bUIOnly)
	{
		FInputModeUIOnly Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(Mode);
		bShowMouseCursor = true;
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}

void AFistBoundPlayerController::ShowMainMenu()
{
	if (UFistBoundUISubsystem* UI = GetUISubsystem())
	{
		UI->PushMenu(MainMenuClass);
	}
	SetUIInputMode(true);
}

void AFistBoundPlayerController::ShowSettings()
{
	if (UFistBoundUISubsystem* UI = GetUISubsystem())
	{
		UI->PushMenu(SettingsMenuClass);
	}
	// Opened from a menu/pause screen that is already UI-only; keep it that way.
	SetUIInputMode(true);
}

void AFistBoundPlayerController::CloseTopMenu()
{
	// Generic "Back": pop only. Does not touch pause state or input mode, so it is
	// safe from a settings/confirm screen layered over the main menu or pause menu.
	if (UFistBoundUISubsystem* UI = GetUISubsystem())
	{
		UI->PopMenu();
	}
}

void AFistBoundPlayerController::TogglePause()
{
	SetPaused(!bIsPaused);
}

void AFistBoundPlayerController::SetPaused(bool bNewPaused)
{
	if (bNewPaused == bIsPaused)
	{
		return;
	}

	bIsPaused = bNewPaused;
	SetPause(bNewPaused);

	UFistBoundUISubsystem* UI = GetUISubsystem();
	if (bNewPaused)
	{
		if (UI)
		{
			UI->PushMenu(PauseMenuClass);
		}
		SetUIInputMode(true);
	}
	else
	{
		if (UI)
		{
			UI->PopMenu();
		}
		SetUIInputMode(false);
	}
}

void AFistBoundPlayerController::StartGame()
{
	if (GameplayLevel.IsNull())
	{
		return;
	}

	// Drop the menu and hand input back to the game before traveling, so nothing
	// lingers into the gameplay level (travel cleanup alone is not reliable, and
	// input would otherwise stay UI-only).
	if (UFistBoundUISubsystem* UI = GetUISubsystem())
	{
		UI->ClearMenus();
	}
	SetUIInputMode(false);

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameplayLevel);
}

void AFistBoundPlayerController::ReturnToMainMenu()
{
	// Clear pause first so the freshly-loaded menu level starts clean.
	SetPaused(false);

	if (!MainMenuLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
	}
}

void AFistBoundPlayerController::ShowConfirmQuit()
{
	if (UFistBoundUISubsystem* UI = GetUISubsystem())
	{
		UI->PushMenu(ConfirmQuitClass);
	}
	SetUIInputMode(true);
}

void AFistBoundPlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}
