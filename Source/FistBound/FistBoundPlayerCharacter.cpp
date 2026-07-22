// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundPlayerCharacter.h"
#include "FistBound.h"
#include "FistBoundHealthComponent.h"
#include "FistBoundTrialSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "TimerManager.h"

namespace
{
	FFistBoundAttackSpec MakeSpec(FName Name, float Windup, float Active, float Recovery, float Damage, float Range, float Radius, float Knockback, FLinearColor Telegraph)
	{
		FFistBoundAttackSpec Spec;
		Spec.Name = Name;
		Spec.WindupSeconds = Windup;
		Spec.ActiveSeconds = Active;
		Spec.RecoverySeconds = Recovery;
		Spec.Damage = Damage;
		Spec.Range = Range;
		Spec.Radius = Radius;
		Spec.KnockbackStrength = Knockback;
		Spec.TelegraphColor = Telegraph;
		return Spec;
	}
}

AFistBoundPlayerCharacter::AFistBoundPlayerCharacter()
{
	StartingHealth = 100.0f;
	BaseColor = FLinearColor(0.16f, 0.45f, 0.95f);
	HitStaggerSeconds = 0.35f;
	DeathLifeSpan = 0.0f; // The corpse stays so the camera has an anchor on the result screen.
	HealthComponent->SetTeam(EFistBoundTeam::Player);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 550.0f;
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->TargetArmLength = 420.0f;
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 70.0f);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 9.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	const FLinearColor LightTelegraph(0.9f, 0.95f, 1.0f);
	LightChain.Add(MakeSpec(TEXT("Light1"), 0.15f, 0.1f, 0.3f, 10.0f, 130.0f, 85.0f, 120.0f, LightTelegraph));
	LightChain.Add(MakeSpec(TEXT("Light2"), 0.12f, 0.1f, 0.3f, 12.0f, 130.0f, 85.0f, 120.0f, LightTelegraph));
	LightChain.Add(MakeSpec(TEXT("Light3"), 0.2f, 0.1f, 0.45f, 16.0f, 150.0f, 95.0f, 300.0f, LightTelegraph));
	HeavyAttack = MakeSpec(TEXT("Heavy"), 0.45f, 0.12f, 0.6f, 26.0f, 160.0f, 100.0f, 550.0f, FLinearColor(0.5f, 0.8f, 1.0f));
}

bool AFistBoundPlayerCharacter::CanAct() const
{
	return Super::CanAct() && !bDodging;
}

void AFistBoundPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BuildTrialInput();

	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Priority 1: layered over the inherited GameForge context (pause).
				Subsystem->AddMappingContext(TrialContext, 1);
				BoundLocalPlayer = LP;
			}
		}
	}

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFistBoundPlayerCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFistBoundPlayerCharacter::Look);
		Input->BindAction(LightAction, ETriggerEvent::Started, this, &AFistBoundPlayerCharacter::OnLightPressed);
		Input->BindAction(HeavyAction, ETriggerEvent::Started, this, &AFistBoundPlayerCharacter::OnHeavyPressed);
		Input->BindAction(DodgeAction, ETriggerEvent::Started, this, &AFistBoundPlayerCharacter::OnDodgePressed);
		Input->BindAction(RestartAction, ETriggerEvent::Started, this, &AFistBoundPlayerCharacter::OnRestartPressed);
		Input->BindAction(MenuAction, ETriggerEvent::Started, this, &AFistBoundPlayerCharacter::OnMenuPressed);
	}
}

void AFistBoundPlayerCharacter::BuildTrialInput()
{
	if (TrialContext)
	{
		return;
	}

	auto MakeAction = [this](const TCHAR* Name, EInputActionValueType ValueType)
	{
		UInputAction* Action = NewObject<UInputAction>(this, Name);
		Action->ValueType = ValueType;
		return Action;
	};

	MoveAction = MakeAction(TEXT("IA_TrialMove"), EInputActionValueType::Axis2D);
	LookAction = MakeAction(TEXT("IA_TrialLook"), EInputActionValueType::Axis2D);
	LightAction = MakeAction(TEXT("IA_TrialLightAttack"), EInputActionValueType::Boolean);
	HeavyAction = MakeAction(TEXT("IA_TrialHeavyAttack"), EInputActionValueType::Boolean);
	DodgeAction = MakeAction(TEXT("IA_TrialDodge"), EInputActionValueType::Boolean);
	RestartAction = MakeAction(TEXT("IA_TrialRestart"), EInputActionValueType::Boolean);
	MenuAction = MakeAction(TEXT("IA_TrialMenu"), EInputActionValueType::Boolean);

	TrialContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Trial"));

	auto AddNegate = [this](FEnhancedActionKeyMapping& Mapping, bool bX, bool bY)
	{
		UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(TrialContext);
		Negate->bX = bX;
		Negate->bY = bY;
		Negate->bZ = false;
		Mapping.Modifiers.Add(Negate);
	};
	auto AddSwizzle = [this](FEnhancedActionKeyMapping& Mapping)
	{
		// Route a 1D key into the Y component of an Axis2D action.
		UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(TrialContext);
		Swizzle->Order = EInputAxisSwizzle::YXZ;
		Mapping.Modifiers.Add(Swizzle);
	};
	auto AddScale = [this](FEnhancedActionKeyMapping& Mapping, float Scale)
	{
		UInputModifierScalar* Scalar = NewObject<UInputModifierScalar>(TrialContext);
		Scalar->Scalar = FVector(Scale);
		Mapping.Modifiers.Add(Scalar);
	};

	// Move: X = right, Y = forward.
	AddSwizzle(TrialContext->MapKey(MoveAction, EKeys::W));
	{
		FEnhancedActionKeyMapping& South = TrialContext->MapKey(MoveAction, EKeys::S);
		AddNegate(South, true, true);
		AddSwizzle(South);
	}
	TrialContext->MapKey(MoveAction, EKeys::D);
	AddNegate(TrialContext->MapKey(MoveAction, EKeys::A), true, true);
	TrialContext->MapKey(MoveAction, EKeys::Gamepad_LeftX);
	AddSwizzle(TrialContext->MapKey(MoveAction, EKeys::Gamepad_LeftY));

	// Look: negate Y so pushing up looks up.
	{
		FEnhancedActionKeyMapping& Mouse = TrialContext->MapKey(LookAction, EKeys::Mouse2D);
		AddNegate(Mouse, false, true);
		AddScale(Mouse, MouseLookScale);
	}
	AddScale(TrialContext->MapKey(LookAction, EKeys::Gamepad_RightX), GamepadLookScale);
	{
		FEnhancedActionKeyMapping& StickY = TrialContext->MapKey(LookAction, EKeys::Gamepad_RightY);
		AddSwizzle(StickY);
		AddNegate(StickY, false, true);
		AddScale(StickY, GamepadLookScale);
	}

	TrialContext->MapKey(LightAction, EKeys::LeftMouseButton);
	TrialContext->MapKey(LightAction, EKeys::Gamepad_FaceButton_Left);
	TrialContext->MapKey(HeavyAction, EKeys::RightMouseButton);
	TrialContext->MapKey(HeavyAction, EKeys::Gamepad_FaceButton_Top);
	TrialContext->MapKey(DodgeAction, EKeys::SpaceBar);
	TrialContext->MapKey(DodgeAction, EKeys::LeftShift);
	TrialContext->MapKey(DodgeAction, EKeys::Gamepad_FaceButton_Right);
	TrialContext->MapKey(RestartAction, EKeys::R);
	TrialContext->MapKey(RestartAction, EKeys::Gamepad_FaceButton_Bottom);
	TrialContext->MapKey(MenuAction, EKeys::M);
	TrialContext->MapKey(MenuAction, EKeys::Gamepad_Special_Left);
}

void AFistBoundPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BoundLocalPlayer.IsValid() && TrialContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = BoundLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->RemoveMappingContext(TrialContext);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AFistBoundPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller || IsDead() || IsStaggered() || IsAttacking() || bDodging)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0, Controller->GetControlRotation().Yaw, 0.0);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right, Axis.X);

	const FVector Direction = Forward * Axis.Y + Right * Axis.X;
	if (!Direction.IsNearlyZero())
	{
		LastMoveDirection = Direction.GetSafeNormal();
	}
}

void AFistBoundPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AFistBoundPlayerCharacter::OnLightPressed()
{
	if (IsAttacking())
	{
		// Inside a chain step: queue the follow-up instead of dropping the press.
		bLightQueued = true;
		return;
	}
	if (CanAct())
	{
		StartChainStep(0);
	}
}

void AFistBoundPlayerCharacter::StartChainStep(int32 StepIndex)
{
	if (!LightChain.IsValidIndex(StepIndex))
	{
		return;
	}
	ChainIndex = StepIndex;
	bLightQueued = false;
	if (Controller)
	{
		// Strike toward the camera facing so attacks land where the player aims.
		SetActorRotation(FRotator(0.0, Controller->GetControlRotation().Yaw, 0.0));
	}
	ExecuteAttack(LightChain[StepIndex]);
}

void AFistBoundPlayerCharacter::OnHeavyPressed()
{
	if (!CanAct())
	{
		return;
	}
	if (Controller)
	{
		SetActorRotation(FRotator(0.0, Controller->GetControlRotation().Yaw, 0.0));
	}
	ExecuteAttack(HeavyAttack);
}

void AFistBoundPlayerCharacter::OnAttackFinished(FName AttackName)
{
	if (bLightQueued && LightChain.IsValidIndex(ChainIndex) && LightChain[ChainIndex].Name == AttackName && LightChain.IsValidIndex(ChainIndex + 1))
	{
		StartChainStep(ChainIndex + 1);
		return;
	}
	ChainIndex = 0;
	bLightQueued = false;
}

void AFistBoundPlayerCharacter::OnDodgePressed()
{
	UWorld* World = GetWorld();
	if (!World || !CanAct() || World->GetTimeSeconds() < NextDodgeTime)
	{
		return;
	}

	NextDodgeTime = World->GetTimeSeconds() + DodgeCooldownSeconds;
	bDodging = true;
	bLightQueued = false;

	const FVector Direction = LastMoveDirection.IsNearlyZero() ? GetActorForwardVector() : LastMoveDirection;
	FaceLocation(GetActorLocation() + Direction * 100.0f);
	LaunchCharacter(Direction * DodgeSpeed, true, false);

	HealthComponent->SetInvulnerable(true);
	SetBodyTint(FLinearColor(0.6f, 1.0f, 1.0f));

	const float InvulnSeconds = FMath::Min(DodgeInvulnerabilitySeconds, DodgeDurationSeconds);
	GetWorldTimerManager().SetTimer(DodgeInvulnTimer, this, &AFistBoundPlayerCharacter::EndDodgeInvulnerability, FMath::Max(InvulnSeconds, 0.01f), false);
	GetWorldTimerManager().SetTimer(DodgeEndTimer, this, &AFistBoundPlayerCharacter::EndDodge, DodgeDurationSeconds, false);
}

void AFistBoundPlayerCharacter::EndDodgeInvulnerability()
{
	HealthComponent->SetInvulnerable(false);
	if (!IsDead() && !IsStaggered())
	{
		SetBodyTint(BaseColor);
	}
}

void AFistBoundPlayerCharacter::EndDodge()
{
	bDodging = false;
}

void AFistBoundPlayerCharacter::OnRestartPressed()
{
	if (UWorld* World = GetWorld())
	{
		if (UFistBoundTrialSubsystem* Trial = World->GetSubsystem<UFistBoundTrialSubsystem>())
		{
			Trial->HandleRestartInput();
		}
	}
}

void AFistBoundPlayerCharacter::OnMenuPressed()
{
	if (UWorld* World = GetWorld())
	{
		if (UFistBoundTrialSubsystem* Trial = World->GetSubsystem<UFistBoundTrialSubsystem>())
		{
			Trial->HandleReturnToMenuInput();
		}
	}
}

void AFistBoundPlayerCharacter::HandleDeath(AActor* DamageCauser)
{
	GetWorldTimerManager().ClearTimer(DodgeInvulnTimer);
	GetWorldTimerManager().ClearTimer(DodgeEndTimer);
	bDodging = false;
	HealthComponent->SetInvulnerable(false);
	Super::HandleDeath(DamageCauser);
}
