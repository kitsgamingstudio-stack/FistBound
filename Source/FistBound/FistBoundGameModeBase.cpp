// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundGameModeBase.h"
#include "FistBoundPlayerController.h"
#include "FistBoundDefaultPawn.h"

AFistBoundGameModeBase::AFistBoundGameModeBase()
{
	PlayerControllerClass = AFistBoundPlayerController::StaticClass();
	DefaultPawnClass = AFistBoundDefaultPawn::StaticClass();
}
