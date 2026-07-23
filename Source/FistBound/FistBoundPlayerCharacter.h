// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FistBoundCombatantCharacter.h"
#include "FistBoundPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class ULocalPlayer;
struct FInputActionValue;

/**
 * The martial-arts trial player: third-person combat camera, movement, a
 * three-hit light chain, one heavy attack, and a dodge with an invulnerability
 * window. Combat input actions and their key mappings are created in C++ at
 * possession time because no game-specific input assets exist yet; the
 * inherited GameForge mapping context (pause) stays active underneath.
 */
UCLASS()
class FISTBOUND_API AFistBoundPlayerCharacter : public AFistBoundCombatantCharacter
{
	GENERATED_BODY()

public:
	AFistBoundPlayerCharacter();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "FistBound|Combat")
	bool IsDodging() const { return bDodging; }

	virtual bool CanAct() const override;

	/** Plays local hit-stop and procedural camera shake for a confirmed hit. */
	void PlayImpactFeedback(const FFistBoundAttackSpec& Attack, bool bReceivedHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnAttackFinished(FName AttackName) override;
	virtual void HandleDeath(AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, Category = "FistBound|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "FistBound|Camera")
	TObjectPtr<UCameraComponent> Camera;

	/** Light chain steps in order; pressing light during a step queues the next. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat")
	TArray<FFistBoundAttackSpec> LightChain;

	UPROPERTY(EditAnywhere, Category = "FistBound|Combat")
	FFistBoundAttackSpec HeavyAttack;

	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float DodgeSpeed = 1500.0f;

	/** Total dodge duration; movement and attacks are locked while dodging. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "0.05"))
	float DodgeDurationSeconds = 0.45f;

	/** Invulnerability window measured from dodge start; must not exceed duration. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float DodgeInvulnerabilitySeconds = 0.3f;

	/** Minimum time between dodge starts. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Combat", meta = (ClampMin = "0.0"))
	float DodgeCooldownSeconds = 0.9f;

	UPROPERTY(EditAnywhere, Category = "FistBound|Input", meta = (ClampMin = "0.01"))
	float MouseLookScale = 0.4f;

	UPROPERTY(EditAnywhere, Category = "FistBound|Input", meta = (ClampMin = "0.01"))
	float GamepadLookScale = 1.0f;

	/** Global dilation used during the short, real-time hit-stop window. */
	UPROPERTY(EditAnywhere, Category = "FistBound|Feedback", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float HitStopTimeDilation = 0.08f;

	UPROPERTY(EditAnywhere, Category = "FistBound|Feedback", meta = (ClampMin = "0.01", ClampMax = "0.5"))
	float ImpactShakeDurationSeconds = 0.16f;

	UPROPERTY(EditAnywhere, Category = "FistBound|Feedback", meta = (ClampMin = "0.0", ClampMax = "30.0"))
	float ImpactShakeLocationAmplitude = 8.0f;

	UPROPERTY(EditAnywhere, Category = "FistBound|Feedback", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float ImpactShakeRotationAmplitude = 1.2f;

private:
	void BuildTrialInput();
	void RestoreImpactFeedback();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnLightPressed();
	void OnHeavyPressed();
	void OnDodgePressed();
	void OnRestartPressed();
	void OnMenuPressed();

	void StartChainStep(int32 StepIndex);
	void EndDodgeInvulnerability();
	void EndDodge();

	// Runtime-built Enhanced Input objects; UPROPERTY keeps them alive under GC.
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> TrialContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LightAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> HeavyAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> RestartAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MenuAction;

	TWeakObjectPtr<ULocalPlayer> BoundLocalPlayer;

	FTimerHandle DodgeInvulnTimer;
	FTimerHandle DodgeEndTimer;

	FVector LastMoveDirection = FVector::ZeroVector;
	int32 ChainIndex = 0;
	bool bLightQueued = false;
	bool bDodging = false;
	float NextDodgeTime = 0.0f;

	FTransform CameraNeutralRelativeTransform;
	double HitStopEndRealSeconds = 0.0;
	double CameraShakeStartRealSeconds = 0.0;
	double CameraShakeEndRealSeconds = 0.0;
	float SavedGlobalTimeDilation = 1.0f;
	float ActiveCameraShakeStrength = 0.0f;
	bool bHitStopActive = false;
	bool bCameraShakeActive = false;
};
