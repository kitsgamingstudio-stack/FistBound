// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FistBoundEnemyCharacter.h"
#include "FistBoundBossCharacter.generated.h"

/**
 * The trial boss. Three attacks with distinct, readable telegraphs:
 *  - Crush: short orange windup, hard forward strike with knockback;
 *  - Flurry: yellow windup into a three-jab chain;
 *  - Slam: long red windup into an area-of-effect burst around the boss.
 * Stagger- and knockback-immune so the light chain cannot stun-lock it; hits
 * still flash for feedback. Supports full reset for checkpoint retries.
 */
UCLASS()
class FISTBOUND_API AFistBoundBossCharacter : public AFistBoundEnemyCharacter
{
	GENERATED_BODY()

public:
	AFistBoundBossCharacter();

	virtual void Tick(float DeltaSeconds) override;

	/** Full heal, teleport home, and return to dormant (player retry from checkpoint). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|AI")
	void ResetForRetry();

protected:
	virtual void BeginPlay() override;
	virtual void OnAttackFinished(FName AttackName) override;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI")
	FFistBoundAttackSpec CrushSpec;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI")
	FFistBoundAttackSpec FlurrySpec;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI")
	FFistBoundAttackSpec SlamSpec;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI", meta = (ClampMin = "0.0"))
	float CrushCooldown = 4.0f;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI", meta = (ClampMin = "0.0"))
	float FlurryCooldown = 6.0f;

	UPROPERTY(EditAnywhere, Category = "FistBound|AI", meta = (ClampMin = "0.0"))
	float SlamCooldown = 9.0f;

private:
	FTransform HomeTransform;
	float NextCrushTime = 0.0f;
	float NextFlurryTime = 0.0f;
	float NextSlamTime = 0.0f;
	int32 FlurryStep = 0;
};
