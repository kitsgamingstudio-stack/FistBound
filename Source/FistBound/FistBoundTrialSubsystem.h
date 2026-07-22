// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FistBoundTrialSubsystem.generated.h"

class AFistBoundBossCharacter;
class AFistBoundEncounter;
class AFistBoundEnemyCharacter;
class AFistBoundPlayerCharacter;
class AStaticMeshActor;
class UMaterialInterface;
class UStaticMesh;

UENUM(BlueprintType)
enum class EFistBoundTrialState : uint8
{
	/** This world is not the trial map, or setup has not run. */
	Inactive,
	Fighting,
	Victory,
	Defeat
};

/**
 * Game-flow coordinator for the Sprint 0001 combat trial.
 *
 * Implemented as a world subsystem keyed on the gameplay map name because the
 * inherited GameForge map pins its GameMode in WorldSettings and GameForge
 * assets must not be edited. On world begin-play in the trial map it builds a
 * self-contained arena above the inherited level, swaps the foundation pawn
 * for the combat player, and drives encounters, checkpoint, boss, victory,
 * defeat, restart, and the completion timer. The HUD and player input query
 * this subsystem; combat rules stay in the combat actors.
 *
 * Victory and Defeat are reachable only from Fighting through one state
 * variable, so they can never hold simultaneously.
 */
UCLASS(Config = Game)
class FISTBOUND_API UFistBoundTrialSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFistBoundTrialSubsystem();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Restart request from player input; valid only on the result states. */
	void HandleRestartInput();

	/** Return-to-menu request from player input; valid only on the result states. */
	void HandleReturnToMenuInput();

	UFUNCTION(BlueprintPure, Category = "FistBound|Trial")
	EFistBoundTrialState GetState() const { return State; }

	/** Trial clock in seconds; frozen on the result states. */
	UFUNCTION(BlueprintPure, Category = "FistBound|Trial")
	float GetElapsedSeconds() const;

	UFUNCTION(BlueprintPure, Category = "FistBound|Trial")
	int32 GetEncountersCompleted() const { return EncountersCompleted; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Trial")
	int32 GetTotalEncounters() const { return Encounters.Num(); }

	UFUNCTION(BlueprintPure, Category = "FistBound|Trial")
	bool IsCheckpointReached() const { return bCheckpointReached; }

	UFUNCTION(BlueprintPure, Category = "FistBound|Trial")
	bool IsBossEngaged() const { return bBossEngaged; }

	AFistBoundBossCharacter* GetBoss() const { return Boss; }
	AFistBoundPlayerCharacter* GetPlayerCharacter() const { return PlayerCharacter; }

	/** Current transient banner text; empty when expired. */
	FString GetCenterMessage() const;

protected:
	/** Short map name (no path/PIE prefix) that hosts the trial. */
	UPROPERTY(Config)
	FString TrialMapName = TEXT("EmptyPlayable");

	/** World-space anchor; the arena floats above the inherited shell level. */
	UPROPERTY(Config)
	FVector ArenaOrigin = FVector(0.0f, 0.0f, 2600.0f);

private:
	void SetupTrial(UWorld& World);
	AStaticMeshActor* SpawnBlock(const FVector& LocalCenter, const FVector& Scale, const FLinearColor& Color);
	void SpawnEncounters();
	void SpawnPlayerAt(const FTransform& SpawnTransform);
	void SetCenterMessage(const FString& Message, float DurationSeconds);
	void PeriodicCheck();
	void RetryFromCheckpoint();
	void ReloadTrial();

	UFUNCTION()
	void HandleEncounterActivated(AFistBoundEncounter* Encounter);

	UFUNCTION()
	void HandleEncounterCompleted(AFistBoundEncounter* Encounter);

	UFUNCTION()
	void HandlePlayerDied(AActor* DamageCauser);

	UFUNCTION()
	void HandleBossDied(AActor* DamageCauser);

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> BlockMaterial;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AFistBoundEncounter>> Encounters;

	UPROPERTY(Transient)
	TObjectPtr<AFistBoundBossCharacter> Boss;

	UPROPERTY(Transient)
	TObjectPtr<AFistBoundPlayerCharacter> PlayerCharacter;

	UPROPERTY(Transient)
	TObjectPtr<AStaticMeshActor> GateWall;

	EFistBoundTrialState State = EFistBoundTrialState::Inactive;
	FTransform StartTransform;
	FTransform CheckpointTransform;
	FTransform BossHomeTransform;

	int32 EncountersCompleted = 0;
	bool bCheckpointReached = false;
	bool bBossEngaged = false;

	/** Completed fight time; the live segment is added while Fighting. */
	float AccumulatedSeconds = 0.0f;
	float SegmentStartSeconds = 0.0f;

	FString CenterMessage;
	float MessageExpireSeconds = 0.0f;

	FTimerHandle PeriodicTimer;
};
