// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FistBoundEncounter.generated.h"

class AFistBoundEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFistBoundEncounterSignature, AFistBoundEncounter*, Encounter);

UENUM()
enum class EFistBoundEncounterState : uint8
{
	Dormant,
	Active,
	Completed
};

/**
 * One bounded combat encounter: spawns its enemies dormant on setup, wakes
 * them when the player comes within the activation radius, and reports
 * completion when every spawned enemy is dead. The trial subsystem places
 * three of these along the corridor and listens for their events.
 */
UCLASS()
class FISTBOUND_API AFistBoundEncounter : public AActor
{
	GENERATED_BODY()

public:
	AFistBoundEncounter();

	/** Spawns the dormant enemy group at the given local-space offsets. */
	void InitEncounter(TSubclassOf<AFistBoundEnemyCharacter> InEnemyClass, const TArray<FVector>& SpawnOffsets);

	UFUNCTION(BlueprintPure, Category = "FistBound|Encounter")
	EFistBoundEncounterState GetState() const { return State; }

	UPROPERTY(BlueprintAssignable, Category = "FistBound|Encounter")
	FFistBoundEncounterSignature OnActivated;

	UPROPERTY(BlueprintAssignable, Category = "FistBound|Encounter")
	FFistBoundEncounterSignature OnCompleted;

	/** Distance from the encounter center at which the group wakes. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Encounter", meta = (ClampMin = "100.0"))
	float ActivationRadius = 700.0f;

protected:
	virtual void BeginPlay() override;

private:
	void CheckActivation();

	UFUNCTION()
	void HandleEnemyDied(AActor* DamageCauser);

	UPROPERTY(Transient)
	TArray<TObjectPtr<AFistBoundEnemyCharacter>> Enemies;

	EFistBoundEncounterState State = EFistBoundEncounterState::Dormant;
	int32 AliveCount = 0;
	FTimerHandle ProximityTimer;
};
