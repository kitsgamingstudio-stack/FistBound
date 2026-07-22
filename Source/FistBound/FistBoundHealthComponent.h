// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FistBoundCombatTypes.h"
#include "FistBoundHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFistBoundDamagedSignature, float, Amount, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFistBoundDiedSignature, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFistBoundHealthChangedSignature, float, Current, float, Max);

/**
 * The narrow damage contract for every FistBound combatant. Attackers and the
 * HUD talk to this component, never to concrete character classes. Owns health,
 * team, and the invulnerability flag; timing of invulnerability windows is the
 * owner's job so this component stays world-independent and unit-testable.
 */
UCLASS(ClassGroup = "FistBound", meta = (BlueprintSpawnableComponent))
class FISTBOUND_API UFistBoundHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFistBoundHealthComponent();

	/** Resets to full health at the given maximum and clears the dead state. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Combat")
	void InitHealth(float InMaxHealth);

	/**
	 * Applies damage unless dead or invulnerable. Returns the damage actually
	 * dealt. Fires OnDamaged, OnHealthChanged, and OnDied (once) as appropriate.
	 */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Combat")
	float ApplyDamage(float Amount, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "FistBound|Combat")
	void SetInvulnerable(bool bInInvulnerable) { bInvulnerable = bInInvulnerable; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	bool IsInvulnerable() const { return bInvulnerable; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	bool IsAlive() const { return !bDead; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	float GetHealthFraction() const { return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	EFistBoundTeam GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable, Category = "FistBound|Combat")
	void SetTeam(EFistBoundTeam InTeam) { Team = InTeam; }

	UPROPERTY(BlueprintAssignable, Category = "FistBound|Combat")
	FFistBoundDamagedSignature OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "FistBound|Combat")
	FFistBoundDiedSignature OnDied;

	UPROPERTY(BlueprintAssignable, Category = "FistBound|Combat")
	FFistBoundHealthChangedSignature OnHealthChanged;

protected:
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "FistBound|Combat")
	EFistBoundTeam Team = EFistBoundTeam::Enemy;

	virtual void BeginPlay() override;

private:
	float CurrentHealth = 100.0f;
	bool bInvulnerable = false;
	bool bDead = false;
};
