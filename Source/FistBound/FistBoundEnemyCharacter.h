// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FistBoundCombatantCharacter.h"
#include "FistBoundEnemyCharacter.generated.h"

/**
 * The standard trial enemy. Runs a minimal tick state machine (dormant ->
 * chase -> attack) driven by direct movement input on a flat arena, so no
 * AIController, behavior tree, or navmesh dependency is needed. Encounters
 * spawn these dormant and call Activate() when the player arrives.
 */
UCLASS()
class FISTBOUND_API AFistBoundEnemyCharacter : public AFistBoundCombatantCharacter
{
	GENERATED_BODY()

public:
	AFistBoundEnemyCharacter();

	virtual void Tick(float DeltaSeconds) override;

	/** Wakes the enemy; it hunts the player until one of them dies. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|AI")
	void Activate();

	UFUNCTION(BlueprintPure, Category = "FistBound|AI")
	bool IsActivated() const { return bActivated; }

protected:
	virtual void OnAttackFinished(FName AttackName) override;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI")
	FFistBoundAttackSpec AttackSpec;

	/** Start attacking when the target is inside this distance. */
	UPROPERTY(EditAnywhere, Category = "FistBound|AI", meta = (ClampMin = "10.0"))
	float EngageRange = 170.0f;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI", meta = (ClampMin = "0.0"))
	float AttackCooldownMin = 0.8f;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI", meta = (ClampMin = "0.0"))
	float AttackCooldownMax = 1.6f;

	/** Finds the live player target, or null when none should be pursued. */
	AFistBoundCombatantCharacter* FindTarget() const;

	/** Returns to the dormant state (boss checkpoint retry). */
	void ResetActivation() { bActivated = false; }

	float NextAttackTime = 0.0f;

private:
	bool bActivated = false;
};
