// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FistBoundCombatTypes.generated.h"

/** Combat allegiance used by attackers to filter hit-query victims. */
UENUM(BlueprintType)
enum class EFistBoundTeam : uint8
{
	Player,
	Enemy
};

/**
 * One melee attack definition: windup -> active (hit queries run) -> recovery.
 * Data-only so player, enemy, and boss attacks share the same execution path
 * and stay tunable from defaults without code changes.
 */
USTRUCT(BlueprintType)
struct FFistBoundAttackSpec
{
	GENERATED_BODY()

	/** Display/log identifier; also used by telegraph diagnostics. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat")
	FName Name = NAME_None;

	/** Telegraph time before the hit becomes active, in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float WindupSeconds = 0.3f;

	/** Duration the hit query stays live, in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat", meta = (ClampMin = "0.01"))
	float ActiveSeconds = 0.1f;

	/** Cooldown after the active window before another action can start. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float RecoverySeconds = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float Damage = 10.0f;

	/** Forward offset from the attacker's location to the hit-sphere center. Ignored for AoE. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float Range = 150.0f;

	/** Radius of the hit-query sphere. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat", meta = (ClampMin = "1.0"))
	float Radius = 90.0f;

	/** Horizontal launch strength applied to victims; 0 = no knockback. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float KnockbackStrength = 0.0f;

	/** Centered on the attacker instead of projected forward (boss ground slam). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat")
	bool bAreaOfEffect = false;

	/** Windup telegraph tint applied to the attacker's placeholder body. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FistBound|Combat")
	FLinearColor TelegraphColor = FLinearColor(1.0f, 0.85f, 0.1f);
};
