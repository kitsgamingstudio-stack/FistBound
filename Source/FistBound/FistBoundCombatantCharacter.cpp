// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundCombatantCharacter.h"
#include "FistBound.h"
#include "FistBoundHealthComponent.h"
#include "FistBoundPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AFistBoundCombatantCharacter::AFistBoundCombatantCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UFistBoundHealthComponent>(TEXT("Health"));

	// Melee combatants steer with movement input and face their travel direction.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		Movement->MaxWalkSpeed = 500.0f;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(GetCapsuleComponent());

	auto MakeShape = [this](const TCHAR* ShapeName, UStaticMesh* InMesh, const FVector& Offset, const FVector& Scale)
	{
		UStaticMeshComponent* Comp = CreateDefaultSubobject<UStaticMeshComponent>(ShapeName);
		Comp->SetupAttachment(VisualRoot);
		if (InMesh)
		{
			Comp->SetStaticMesh(InMesh);
		}
		if (ShapeMaterial.Succeeded())
		{
			Comp->SetMaterial(0, ShapeMaterial.Object);
		}
		Comp->SetRelativeLocation(Offset);
		Comp->SetRelativeScale3D(Scale);
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetGenerateOverlapEvents(false);
		Comp->SetCanEverAffectNavigation(false);
		return Comp;
	};

	// Placeholder body: torso block, head sphere, and a leading "fist" sphere that
	// swells during windup so attacks read even without animation assets.
	TorsoMesh = MakeShape(TEXT("Torso"), CubeMesh.Object, FVector(0.0f, 0.0f, -10.0f), FVector(0.55f, 0.75f, 1.35f));
	HeadMesh = MakeShape(TEXT("Head"), SphereMesh.Object, FVector(0.0f, 0.0f, 78.0f), FVector(0.42f, 0.42f, 0.42f));
	FistMesh = MakeShape(TEXT("Fist"), SphereMesh.Object, FVector(55.0f, 22.0f, 20.0f), FVector(0.28f, 0.28f, 0.28f));
}

void AFistBoundCombatantCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto MakeMID = [](UStaticMeshComponent* Comp) -> UMaterialInstanceDynamic*
	{
		return Comp && Comp->GetMaterial(0) ? Comp->CreateAndSetMaterialInstanceDynamic(0) : nullptr;
	};
	BodyMID = MakeMID(TorsoMesh);
	FistMID = MakeMID(FistMesh);
	if (HeadMesh && BodyMID)
	{
		HeadMesh->SetMaterial(0, BodyMID);
	}
	SetBodyTint(BaseColor);
	if (FistMID)
	{
		FistMID->SetVectorParameterValue(TEXT("Color"), BaseColor * 0.6f);
	}

	HealthComponent->InitHealth(StartingHealth);
	HealthComponent->OnDamaged.AddDynamic(this, &AFistBoundCombatantCharacter::OnHealthDamaged);
	HealthComponent->OnDied.AddDynamic(this, &AFistBoundCombatantCharacter::OnHealthDied);
}

bool AFistBoundCombatantCharacter::IsDead() const
{
	return HealthComponent && !HealthComponent->IsAlive();
}

bool AFistBoundCombatantCharacter::CanAct() const
{
	return !bAttacking && !bStaggered && !IsDead();
}

void AFistBoundCombatantCharacter::FaceLocation(const FVector& WorldLocation)
{
	FVector ToTarget = WorldLocation - GetActorLocation();
	ToTarget.Z = 0.0f;
	if (!ToTarget.IsNearlyZero())
	{
		SetActorRotation(ToTarget.Rotation());
	}
}

bool AFistBoundCombatantCharacter::ExecuteAttack(const FFistBoundAttackSpec& Spec)
{
	if (!CanAct())
	{
		return false;
	}

	bAttacking = true;
	CurrentAttack = Spec;
	HitActorsThisAttack.Reset();

	SetBodyTint(Spec.TelegraphColor);
	if (FistMesh)
	{
		FistMesh->SetRelativeScale3D(FVector(0.45f));
	}

	GetWorldTimerManager().SetTimer(WindupTimer, this, &AFistBoundCombatantCharacter::StartActiveWindow, FMath::Max(Spec.WindupSeconds, 0.01f), false);
	return true;
}

void AFistBoundCombatantCharacter::StartActiveWindow()
{
	SetBodyTint(BaseColor);

	// Repeating pulse so a moving attacker still connects during the window.
	DoHitPulse();
	GetWorldTimerManager().SetTimer(HitPulseTimer, this, &AFistBoundCombatantCharacter::DoHitPulse, 0.05f, true);
	GetWorldTimerManager().SetTimer(ActiveEndTimer, this, &AFistBoundCombatantCharacter::EndActiveWindow, CurrentAttack.ActiveSeconds, false);
}

void AFistBoundCombatantCharacter::EndActiveWindow()
{
	GetWorldTimerManager().ClearTimer(HitPulseTimer);
	if (FistMesh)
	{
		FistMesh->SetRelativeScale3D(FVector(0.28f));
	}
	GetWorldTimerManager().SetTimer(RecoveryTimer, this, &AFistBoundCombatantCharacter::FinishAttack, FMath::Max(CurrentAttack.RecoverySeconds, 0.01f), false);
}

void AFistBoundCombatantCharacter::FinishAttack()
{
	bAttacking = false;
	OnAttackFinished(CurrentAttack.Name);
}

void AFistBoundCombatantCharacter::DoHitPulse()
{
	UWorld* World = GetWorld();
	if (!World || !HealthComponent)
	{
		return;
	}

	const FVector Center = CurrentAttack.bAreaOfEffect
		? GetActorLocation()
		: GetActorLocation() + GetActorForwardVector() * CurrentAttack.Range;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FistBoundHitPulse), false, this);
	World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(CurrentAttack.Radius), Params);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Victim = Overlap.GetActor();
		if (!Victim || Victim == this || HitActorsThisAttack.Contains(Victim))
		{
			continue;
		}

		UFistBoundHealthComponent* VictimHealth = Victim->FindComponentByClass<UFistBoundHealthComponent>();
		if (!VictimHealth || VictimHealth->GetTeam() == HealthComponent->GetTeam() || !VictimHealth->IsAlive())
		{
			continue;
		}

		HitActorsThisAttack.Add(Victim);
		const float Applied = VictimHealth->ApplyDamage(CurrentAttack.Damage, this);

		if (Applied > 0.0f)
		{
			// Impact feedback belongs to the local player's experience. Enemy-only
			// exchanges stay silent, and invulnerable dodges never reach this branch.
			AFistBoundPlayerCharacter* LocalPlayer = Cast<AFistBoundPlayerCharacter>(this);
			if (!LocalPlayer)
			{
				LocalPlayer = Cast<AFistBoundPlayerCharacter>(Victim);
			}
			if (LocalPlayer && LocalPlayer->IsLocallyControlled())
			{
				LocalPlayer->PlayImpactFeedback(CurrentAttack, Victim == LocalPlayer);
			}

			if (CurrentAttack.KnockbackStrength > 0.0f)
			{
				if (AFistBoundCombatantCharacter* VictimCharacter = Cast<AFistBoundCombatantCharacter>(Victim))
				{
					if (!VictimCharacter->bKnockbackImmune)
					{
						FVector Push = Victim->GetActorLocation() - GetActorLocation();
						Push.Z = 0.0f;
						Push = Push.GetSafeNormal() * CurrentAttack.KnockbackStrength + FVector(0.0f, 0.0f, CurrentAttack.KnockbackStrength * 0.25f);
						VictimCharacter->LaunchCharacter(Push, true, false);
					}
				}
			}
		}
	}
}

void AFistBoundCombatantCharacter::CancelAttack()
{
	FTimerManager& Timers = GetWorldTimerManager();
	Timers.ClearTimer(WindupTimer);
	Timers.ClearTimer(ActiveEndTimer);
	Timers.ClearTimer(HitPulseTimer);
	Timers.ClearTimer(RecoveryTimer);
	bAttacking = false;
	SetBodyTint(BaseColor);
	if (FistMesh)
	{
		FistMesh->SetRelativeScale3D(FVector(0.28f));
	}
}

void AFistBoundCombatantCharacter::Stagger(float DurationSeconds)
{
	if (IsDead() || DurationSeconds <= 0.0f)
	{
		return;
	}

	CancelAttack();
	bStaggered = true;
	SetBodyTint(FLinearColor::White);
	GetWorldTimerManager().SetTimer(StaggerTimer, this, &AFistBoundCombatantCharacter::ClearStagger, DurationSeconds, false);
}

void AFistBoundCombatantCharacter::ClearStagger()
{
	bStaggered = false;
	if (!IsDead())
	{
		SetBodyTint(BaseColor);
	}
}

void AFistBoundCombatantCharacter::OnHealthDamaged(float Amount, AActor* DamageCauser)
{
	HandleDamaged(Amount, DamageCauser);
}

void AFistBoundCombatantCharacter::OnHealthDied(AActor* DamageCauser)
{
	HandleDeath(DamageCauser);
}

void AFistBoundCombatantCharacter::HandleDamaged(float Amount, AActor* DamageCauser)
{
	if (IsDead())
	{
		return;
	}

	if (HitStaggerSeconds > 0.0f)
	{
		Stagger(HitStaggerSeconds);
	}
	else
	{
		// Stagger-immune combatants still flash so hits give feedback.
		SetBodyTint(FLinearColor::White);
		FTimerHandle FlashTimer;
		GetWorldTimerManager().SetTimer(FlashTimer, this, &AFistBoundCombatantCharacter::ClearStagger, 0.08f, false);
	}
}

void AFistBoundCombatantCharacter::HandleDeath(AActor* DamageCauser)
{
	CancelAttack();
	GetWorldTimerManager().ClearTimer(StaggerTimer);
	bStaggered = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	if (VisualRoot)
	{
		VisualRoot->SetRelativeRotation(FRotator(-80.0f, 0.0f, 0.0f));
		VisualRoot->SetRelativeLocation(FVector(0.0f, 0.0f, -55.0f));
	}
	SetBodyTint(FLinearColor(0.12f, 0.12f, 0.12f));

	if (DeathLifeSpan > 0.0f)
	{
		SetLifeSpan(DeathLifeSpan);
	}

	UE_LOG(LogFistBound, Log, TEXT("%s died (killer: %s)"), *GetName(), DamageCauser ? *DamageCauser->GetName() : TEXT("none"));
}

void AFistBoundCombatantCharacter::SetBodyTint(const FLinearColor& Color)
{
	if (BodyMID)
	{
		BodyMID->SetVectorParameterValue(TEXT("Color"), Color);
	}
}
