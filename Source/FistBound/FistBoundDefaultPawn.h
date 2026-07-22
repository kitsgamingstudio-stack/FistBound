// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "FistBoundDefaultPawn.generated.h"

class UInputAction;
struct FInputActionValue;

/**
 * Lightweight spectator-style pawn used by the foundation so the boot map is
 * instantly navigable. Legacy movement bindings are disabled because FistBound
 * standardises on Enhanced Input; move/look are bound from the assigned actions.
 * Derived games replace this with their own pawn.
 */
UCLASS()
class FISTBOUND_API AFistBoundDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	AFistBoundDefaultPawn();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	/** Axis2D: planar movement relative to control yaw. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Input")
	TObjectPtr<UInputAction> MoveAction;

	/** Axis2D: yaw/pitch look. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Input")
	TObjectPtr<UInputAction> LookAction;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
};
