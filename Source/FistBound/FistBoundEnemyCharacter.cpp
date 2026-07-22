// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundEnemyCharacter.h"
#include "FistBoundHealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AFistBoundEnemyCharacter::AFistBoundEnemyCharacter()
{
	StartingHealth = 30.0f;
	BaseColor = FLinearColor(0.85f, 0.2f, 0.15f);
	HitStaggerSeconds = 0.35f;
	DeathLifeSpan = 3.0f;
	HealthComponent->SetTeam(EFistBoundTeam::Enemy);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 380.0f;
		// Enemies are driven by direct movement input without a controller.
		Movement->bRunPhysicsWithNoController = true;
	}
	AutoPossessAI = EAutoPossessAI::Disabled;

	AttackSpec.Name = TEXT("Jab");
	AttackSpec.WindupSeconds = 0.5f;
	AttackSpec.ActiveSeconds = 0.12f;
	AttackSpec.RecoverySeconds = 0.6f;
	AttackSpec.Damage = 10.0f;
	AttackSpec.Range = 140.0f;
	AttackSpec.Radius = 80.0f;
	AttackSpec.KnockbackStrength = 150.0f;
	AttackSpec.TelegraphColor = FLinearColor(1.0f, 0.55f, 0.1f);
}

void AFistBoundEnemyCharacter::Activate()
{
	if (bActivated || IsDead())
	{
		return;
	}
	bActivated = true;
	if (const UWorld* World = GetWorld())
	{
		// Grace period so a freshly woken pack does not punch instantly.
		NextAttackTime = World->GetTimeSeconds() + FMath::FRandRange(0.5f, 1.2f);
	}
}

AFistBoundCombatantCharacter* AFistBoundEnemyCharacter::FindTarget() const
{
	AFistBoundCombatantCharacter* Target = Cast<AFistBoundCombatantCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	return (Target && !Target->IsDead()) ? Target : nullptr;
}

void AFistBoundEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bActivated || IsDead() || IsStaggered() || IsAttacking())
	{
		return;
	}

	AFistBoundCombatantCharacter* Target = FindTarget();
	if (!Target)
	{
		return;
	}

	const FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	const float Distance = ToTarget.Size2D();

	if (Distance > EngageRange)
	{
		AddMovementInput(ToTarget.GetSafeNormal2D());
	}
	else if (GetWorld()->GetTimeSeconds() >= NextAttackTime)
	{
		FaceLocation(Target->GetActorLocation());
		ExecuteAttack(AttackSpec);
	}
}

void AFistBoundEnemyCharacter::OnAttackFinished(FName AttackName)
{
	Super::OnAttackFinished(AttackName);
	if (const UWorld* World = GetWorld())
	{
		NextAttackTime = World->GetTimeSeconds() + FMath::FRandRange(AttackCooldownMin, AttackCooldownMax);
	}
}
