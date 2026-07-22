// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FistBoundCombatTypes.h"
#include "FistBoundCombatantCharacter.generated.h"

class UFistBoundHealthComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/**
 * Shared base for every FistBound combatant (player, enemy, boss).
 *
 * Owns the timed attack pipeline (windup -> active hit pulses -> recovery),
 * stagger, death handling, and the primitive placeholder visuals. Hit detection
 * is a sphere overlap in front of the attacker during the active window — the
 * smallest approach that supports the trial; anim-notify-driven hitboxes can
 * replace it once real animation assets exist. Telegraphs are shown by tinting
 * the body with the attack's telegraph color for the whole windup.
 */
UCLASS(Abstract)
class FISTBOUND_API AFistBoundCombatantCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFistBoundCombatantCharacter();

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	UFistBoundHealthComponent* GetHealthComponent() const { return HealthComponent; }

	/** Starts an attack if the combatant can act. Returns false when busy/dead. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Combat")
	bool ExecuteAttack(const FFistBoundAttackSpec& Spec);

	/** Interrupts any attack in progress and locks actions for the duration. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Combat")
	void Stagger(float DurationSeconds);

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	bool IsAttacking() const { return bAttacking; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	bool IsStaggered() const { return bStaggered; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	bool IsDead() const;

	/** True when no attack, stagger, or death blocks a new action. */
	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	virtual bool CanAct() const;

	/** Yaw-only turn toward a world location (attack facing, AI steering). */
	void FaceLocation(const FVector& WorldLocation);

protected:
	virtual void BeginPlay() override;

	/** Called after recovery completes; subclasses chain or set cooldowns here. */
	virtual void OnAttackFinished(FName AttackName) {}

	/** Victim-side reaction to damage; base flashes and applies HitStaggerSeconds. */
	virtual void HandleDamaged(float Amount, AActor* DamageCauser);

	virtual void HandleDeath(AActor* DamageCauser);

	/** Tints the placeholder body; pass BaseColor to restore. */
	void SetBodyTint(const FLinearColor& Color);

	UPROPERTY(VisibleAnywhere, Category = "FistBound|Combat")
	TObjectPtr<UFistBoundHealthComponent> HealthComponent;

	/** Root for the placeholder shapes so death can topple them independently of the capsule. */
	UPROPERTY(VisibleAnywhere, Category = "FistBound|Visual")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(VisibleAnywhere, Category = "FistBound|Visual")
	TObjectPtr<UStaticMeshComponent> TorsoMesh;

	UPROPERTY(VisibleAnywhere, Category = "FistBound|Visual")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, Category = "FistBound|Visual")
	TObjectPtr<UStaticMeshComponent> FistMesh;

	/** Resting tint of the placeholder body; set per subclass. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Visual")
	FLinearColor BaseColor = FLinearColor::Gray;

	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "1.0"))
	float StartingHealth = 100.0f;

	/** Stagger this combatant suffers per received hit; 0 = stagger-immune (boss). */
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float HitStaggerSeconds = 0.35f;

	/** Ignore attack knockback (boss). */
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat")
	bool bKnockbackImmune = false;

	/** Corpse lifetime; 0 keeps it forever (player corpse stays for the camera). */
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float DeathLifeSpan = 0.0f;

	/** Cancels every attack/stagger timer and restores neutral visuals. */
	void CancelAttack();

private:
	void StartActiveWindow();
	void EndActiveWindow();
	void FinishAttack();
	void DoHitPulse();
	void ClearStagger();

	UFUNCTION()
	void OnHealthDamaged(float Amount, AActor* DamageCauser);

	UFUNCTION()
	void OnHealthDied(AActor* DamageCauser);

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> BodyMID;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> FistMID;

	FFistBoundAttackSpec CurrentAttack;
	TSet<TObjectPtr<AActor>> HitActorsThisAttack;

	FTimerHandle WindupTimer;
	FTimerHandle ActiveEndTimer;
	FTimerHandle HitPulseTimer;
	FTimerHandle RecoveryTimer;
	FTimerHandle StaggerTimer;

	bool bAttacking = false;
	bool bStaggered = false;
};
