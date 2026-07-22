// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FistBoundGameModeBase.generated.h"

/**
 * Genre-neutral GameMode for FistBound-derived games.
 * Wires the foundation PlayerController and a fly-around DefaultPawn so a
 * derived game boots into something immediately playable. Derived games
 * subclass this (in C++ or Blueprint) and swap the pawn for their own.
 */
UCLASS()
class FISTBOUND_API AFistBoundGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFistBoundGameModeBase();
};
