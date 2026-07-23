// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundBossCharacter.h"
#include "FistBound.h"
#include "FistBoundHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

AFistBoundBossCharacter::AFistBoundBossCharacter()
{
	StartingHealth = 220.0f;
	BaseColor = FLinearColor(0.42f, 0.07f, 0.07f);
	HitStaggerSeconds = 0.0f; // Stagger-immune; hits flash instead so the light chain cannot stun-lock.
	bKnockbackImmune = true;
	DeathLifeSpan = 0.0f; // The fallen boss stays as the victory backdrop.

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 300.0f;
	}
	SetActorScale3D(FVector(1.35f));

	CrushSpec.Name = TEXT("Crush");
	CrushSpec.WindupSeconds = 0.85f;
	CrushSpec.ActiveSeconds = 0.15f;
	CrushSpec.RecoverySeconds = 0.9f;
	CrushSpec.Damage = 24.0f;
	CrushSpec.Range = 200.0f;
	CrushSpec.Radius = 110.0f;
	CrushSpec.KnockbackStrength = 500.0f;
	CrushSpec.HitStopSeconds = 0.055f;
	CrushSpec.CameraShakeStrength = 0.9f;
	CrushSpec.TelegraphColor = FLinearColor(1.0f, 0.5f, 0.05f);

	FlurrySpec.Name = TEXT("Flurry");
	FlurrySpec.WindupSeconds = 0.55f;
	FlurrySpec.ActiveSeconds = 0.1f;
	FlurrySpec.RecoverySeconds = 0.25f;
	FlurrySpec.Damage = 8.0f;
	FlurrySpec.Range = 180.0f;
	FlurrySpec.Radius = 90.0f;
	FlurrySpec.KnockbackStrength = 100.0f;
	FlurrySpec.HitStopSeconds = 0.025f;
	FlurrySpec.CameraShakeStrength = 0.35f;
	FlurrySpec.TelegraphColor = FLinearColor(1.0f, 0.9f, 0.1f);

	SlamSpec.Name = TEXT("Slam");
	SlamSpec.WindupSeconds = 1.2f;
	SlamSpec.ActiveSeconds = 0.2f;
	SlamSpec.RecoverySeconds = 1.1f;
	SlamSpec.Damage = 32.0f;
	SlamSpec.Radius = 420.0f;
	SlamSpec.KnockbackStrength = 700.0f;
	SlamSpec.HitStopSeconds = 0.075f;
	SlamSpec.CameraShakeStrength = 1.25f;
	SlamSpec.bAreaOfEffect = true;
	SlamSpec.TelegraphColor = FLinearColor(1.0f, 0.1f, 0.05f);
}

void AFistBoundBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	HomeTransform = GetActorTransform();
}

void AFistBoundBossCharacter::Tick(float DeltaSeconds)
{
	// Skip the standard-enemy brain; ACharacter::Tick is enough underneath.
	ACharacter::Tick(DeltaSeconds);

	if (!IsActivated() || IsDead() || IsStaggered() || IsAttacking())
	{
		return;
	}

	AFistBoundCombatantCharacter* Target = FindTarget();
	if (!Target)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const FVector TargetLocation = Target->GetActorLocation();
	const float Distance = (TargetLocation - GetActorLocation()).Size2D();

	if (Now >= NextAttackTime)
	{
		// Longest-telegraph attack gets priority so players see it often enough to learn it.
		if (Distance <= 400.0f && Now >= NextSlamTime)
		{
			NextSlamTime = Now + SlamCooldown;
			FaceLocation(TargetLocation);
			ExecuteAttack(SlamSpec);
			return;
		}
		if (Distance <= 300.0f && Now >= NextFlurryTime)
		{
			NextFlurryTime = Now + FlurryCooldown;
			FlurryStep = 0;
			FaceLocation(TargetLocation);
			ExecuteAttack(FlurrySpec);
			return;
		}
		if (Distance <= 280.0f && Now >= NextCrushTime)
		{
			NextCrushTime = Now + CrushCooldown;
			FaceLocation(TargetLocation);
			ExecuteAttack(CrushSpec);
			return;
		}
	}

	if (Distance > 200.0f)
	{
		AddMovementInput((TargetLocation - GetActorLocation()).GetSafeNormal2D());
	}
	else
	{
		FaceLocation(TargetLocation);
	}
}

void AFistBoundBossCharacter::OnAttackFinished(FName AttackName)
{
	if (AttackName == FlurrySpec.Name && FlurryStep < 2)
	{
		++FlurryStep;
		if (AFistBoundCombatantCharacter* Target = FindTarget())
		{
			FaceLocation(Target->GetActorLocation());
		}
		FFistBoundAttackSpec Step = FlurrySpec;
		Step.WindupSeconds = 0.18f; // Follow-up jabs come fast; the read is the first windup.
		ExecuteAttack(Step);
		return;
	}

	FlurryStep = 0;
	if (const UWorld* World = GetWorld())
	{
		NextAttackTime = World->GetTimeSeconds() + FMath::FRandRange(1.3f, 2.1f);
	}
}

void AFistBoundBossCharacter::ResetForRetry()
{
	CancelAttack();
	FlurryStep = 0;
	ResetActivation();

	HealthComponent->InitHealth(StartingHealth);
	SetActorTransform(HomeTransform);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->SetMovementMode(MOVE_Walking);
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
	if (VisualRoot)
	{
		VisualRoot->SetRelativeRotation(FRotator::ZeroRotator);
		VisualRoot->SetRelativeLocation(FVector::ZeroVector);
	}
	SetBodyTint(BaseColor);

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	NextAttackTime = Now + 1.0f;
	NextCrushTime = Now;
	NextFlurryTime = Now;
	NextSlamTime = Now;

	UE_LOG(LogFistBound, Log, TEXT("Boss reset for checkpoint retry"));
}
