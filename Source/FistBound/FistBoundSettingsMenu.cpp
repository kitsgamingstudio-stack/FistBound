// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundSettingsMenu.h"
#include "FistBoundSaveSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UFistBoundSaveSubsystem* UFistBoundSettingsMenu::GetSaveSubsystem() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UGameInstance* GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<UFistBoundSaveSubsystem>();
		}
	}
	return nullptr;
}
