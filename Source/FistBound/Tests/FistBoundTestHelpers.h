// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FistBoundTestHelpers.generated.h"

/** Counts dynamic-delegate broadcasts for automation tests. */
UCLASS()
class UFistBoundTestDeathListener : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleDied(AActor* DamageCauser) { ++DeathCount; }

	int32 DeathCount = 0;
};
