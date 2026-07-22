// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FistBoundSaveSubsystem.generated.h"

class UFistBoundSaveGame;

/**
 * Foundation settings/save stub. Loads (or creates) a single settings slot on
 * startup and exposes minimal save/load so any derived game has a working
 * persistence path from day one. Derived games extend the save class and API.
 */
UCLASS()
class FISTBOUND_API UFistBoundSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintPure, Category = "FistBound|Settings")
	UFistBoundSaveGame* GetSettings() const { return CurrentSave; }

	/** Writes the in-memory settings object to the slot. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Settings")
	bool SaveSettings();

	/** Reads the slot into the in-memory settings object (creating defaults if none). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Settings")
	bool LoadSettings();

	/** Pushes the in-memory settings to the engine (audio volume, scalability). */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Settings")
	void ApplySettings();

	/** Sets and live-applies the master volume (0..1). Does not persist. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Settings")
	void SetMasterVolume(float InVolume);

	/** Sets and live-applies the overall graphics/scalability level (0..4). Does not persist. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|Settings")
	void SetGraphicsQuality(int32 InQuality);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "FistBound|Settings")
	FString SlotName = TEXT("FistBoundSettings");

	UPROPERTY(EditDefaultsOnly, Category = "FistBound|Settings")
	int32 UserIndex = 0;

	UPROPERTY(Transient)
	TObjectPtr<UFistBoundSaveGame> CurrentSave = nullptr;

private:
	void ApplyAudioSettings();
	void ApplyGraphicsSettings();
};
