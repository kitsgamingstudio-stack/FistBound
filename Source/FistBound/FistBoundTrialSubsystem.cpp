// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundTrialSubsystem.h"
#include "FistBound.h"
#include "FistBoundBossCharacter.h"
#include "FistBoundEncounter.h"
#include "FistBoundEnemyCharacter.h"
#include "FistBoundHealthComponent.h"
#include "FistBoundPlayerCharacter.h"
#include "FistBoundPlayerController.h"
#include "FistBoundTrialHUD.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/PackageName.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

namespace FistBoundTrial
{
	// Arena layout in arena-local units: a corridor along +X with three
	// encounters, a gate that opens after the third, a checkpoint pad, and a
	// wider boss ring at the end.
	constexpr float FloorTopZ = 0.0f;
	const FVector PlayerStartLocal(150.0f, 0.0f, 120.0f);
	const FVector CheckpointLocal(4700.0f, 0.0f, 120.0f);
	const FVector BossHomeLocal(6100.0f, 0.0f, 140.0f);
	const FVector EncounterCentersLocal[3] = { FVector(1500.0f, 0.0f, 0.0f), FVector(2700.0f, 0.0f, 0.0f), FVector(3900.0f, 0.0f, 0.0f) };
	constexpr float BossEngageDistance = 900.0f;
	constexpr float FallRecoveryDepth = 400.0f;
}

UFistBoundTrialSubsystem::UFistBoundTrialSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	CubeMesh = Cube.Object;
	BlockMaterial = Material.Object;
}

bool UFistBoundTrialSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UFistBoundTrialSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const FString ShortMapName = FPackageName::GetShortName(UWorld::RemovePIEPrefix(InWorld.GetOutermost()->GetName()));
	if (!ShortMapName.Equals(TrialMapName, ESearchCase::IgnoreCase))
	{
		return;
	}

	UE_LOG(LogFistBound, Log, TEXT("Trial subsystem: setting up combat trial in %s"), *ShortMapName);
	SetupTrial(InWorld);
}

void UFistBoundTrialSubsystem::SetupTrial(UWorld& World)
{
	using namespace FistBoundTrial;

	// --- Arena geometry -------------------------------------------------
	const FLinearColor FloorColor(0.22f, 0.22f, 0.26f);
	const FLinearColor WallColor(0.35f, 0.3f, 0.24f);
	const FLinearColor GateColor(0.55f, 0.12f, 0.1f);
	const FLinearColor CheckpointColor(0.95f, 0.78f, 0.2f);

	SpawnBlock(FVector(2250.0f, 0.0f, -50.0f), FVector(52.0f, 12.0f, 1.0f), FloorColor);      // corridor floor
	SpawnBlock(FVector(5950.0f, 0.0f, -50.0f), FVector(28.0f, 22.0f, 1.0f), FloorColor);      // boss ring floor
	SpawnBlock(FVector(2250.0f, 620.0f, 150.0f), FVector(52.0f, 0.4f, 4.0f), WallColor);      // corridor walls
	SpawnBlock(FVector(2250.0f, -620.0f, 150.0f), FVector(52.0f, 0.4f, 4.0f), WallColor);
	SpawnBlock(FVector(-370.0f, 0.0f, 150.0f), FVector(0.4f, 12.0f, 4.0f), WallColor);        // back wall
	SpawnBlock(FVector(5950.0f, 1120.0f, 150.0f), FVector(28.0f, 0.4f, 4.0f), WallColor);     // boss ring walls
	SpawnBlock(FVector(5950.0f, -1120.0f, 150.0f), FVector(28.0f, 0.4f, 4.0f), WallColor);
	SpawnBlock(FVector(7370.0f, 0.0f, 150.0f), FVector(0.4f, 22.0f, 4.0f), WallColor);        // far wall
	GateWall = SpawnBlock(FVector(4450.0f, 0.0f, 150.0f), FVector(0.5f, 12.0f, 4.0f), GateColor);
	SpawnBlock(FVector(4700.0f, 0.0f, 4.0f), FVector(1.6f, 1.6f, 0.08f), CheckpointColor);    // checkpoint pad

	StartTransform = FTransform(FRotator::ZeroRotator, ArenaOrigin + PlayerStartLocal);
	CheckpointTransform = FTransform(FRotator::ZeroRotator, ArenaOrigin + CheckpointLocal);
	BossHomeTransform = FTransform(FRotator(0.0f, 180.0f, 0.0f), ArenaOrigin + BossHomeLocal);

	// --- Encounters and boss -------------------------------------------
	SpawnEncounters();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Boss = World.SpawnActor<AFistBoundBossCharacter>(AFistBoundBossCharacter::StaticClass(), BossHomeTransform, SpawnParams);
	if (Boss)
	{
		Boss->GetHealthComponent()->OnDied.AddDynamic(this, &UFistBoundTrialSubsystem::HandleBossDied);
	}

	// --- Player and HUD -------------------------------------------------
	SpawnPlayerAt(StartTransform);
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(&World, 0))
	{
		PC->ClientSetHUD(AFistBoundTrialHUD::StaticClass());
	}

	// --- Flow -----------------------------------------------------------
	State = EFistBoundTrialState::Fighting;
	AccumulatedSeconds = 0.0f;
	SegmentStartSeconds = World.GetTimeSeconds();
	SetCenterMessage(TEXT("THE TRIAL BEGINS"), 4.0f);

	World.GetTimerManager().SetTimer(PeriodicTimer, this, &UFistBoundTrialSubsystem::PeriodicCheck, 0.5f, true);
}

AStaticMeshActor* UFistBoundTrialSubsystem::SpawnBlock(const FVector& LocalCenter, const FVector& Scale, const FLinearColor& Color)
{
	UWorld* World = GetWorld();
	if (!World || !CubeMesh)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Block = World->SpawnActor<AStaticMeshActor>(ArenaOrigin + LocalCenter, FRotator::ZeroRotator, SpawnParams);
	if (!Block)
	{
		return nullptr;
	}

	Block->SetMobility(EComponentMobility::Movable);
	if (UStaticMeshComponent* Mesh = Block->GetStaticMeshComponent())
	{
		Mesh->SetStaticMesh(CubeMesh);
		if (BlockMaterial)
		{
			Mesh->SetMaterial(0, BlockMaterial);
			if (UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(0))
			{
				MID->SetVectorParameterValue(TEXT("Color"), Color);
			}
		}
	}
	Block->SetActorScale3D(Scale);
	return Block;
}

void UFistBoundTrialSubsystem::SpawnEncounters()
{
	using namespace FistBoundTrial;
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Group sizes ramp 2 -> 3 -> 4 across the corridor.
	const TArray<TArray<FVector>> SpawnOffsets = {
		{ FVector(150.0f, 130.0f, 120.0f), FVector(150.0f, -130.0f, 120.0f) },
		{ FVector(120.0f, 200.0f, 120.0f), FVector(220.0f, 0.0f, 120.0f), FVector(120.0f, -200.0f, 120.0f) },
		{ FVector(120.0f, 220.0f, 120.0f), FVector(240.0f, 90.0f, 120.0f), FVector(240.0f, -90.0f, 120.0f), FVector(120.0f, -220.0f, 120.0f) }
	};

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 Index = 0; Index < 3; ++Index)
	{
		const FVector Center = ArenaOrigin + EncounterCentersLocal[Index];
		AFistBoundEncounter* Encounter = World->SpawnActor<AFistBoundEncounter>(Center, FRotator::ZeroRotator, SpawnParams);
		if (!Encounter)
		{
			continue;
		}
		Encounter->OnActivated.AddDynamic(this, &UFistBoundTrialSubsystem::HandleEncounterActivated);
		Encounter->OnCompleted.AddDynamic(this, &UFistBoundTrialSubsystem::HandleEncounterCompleted);
		Encounter->InitEncounter(AFistBoundEnemyCharacter::StaticClass(), SpawnOffsets[Index]);
		Encounters.Add(Encounter);
	}
}

void UFistBoundTrialSubsystem::SpawnPlayerAt(const FTransform& SpawnTransform)
{
	UWorld* World = GetWorld();
	APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
	if (!World || !PC)
	{
		UE_LOG(LogFistBound, Warning, TEXT("Trial subsystem: no player controller; trial not playable"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AFistBoundPlayerCharacter* NewCharacter = World->SpawnActor<AFistBoundPlayerCharacter>(AFistBoundPlayerCharacter::StaticClass(), SpawnTransform, SpawnParams);
	if (!NewCharacter)
	{
		UE_LOG(LogFistBound, Error, TEXT("Trial subsystem: failed to spawn player character"));
		return;
	}

	APawn* OldPawn = PC->GetPawn();
	PC->Possess(NewCharacter);
	if (OldPawn && OldPawn != NewCharacter)
	{
		OldPawn->Destroy();
	}

	NewCharacter->GetHealthComponent()->OnDied.AddDynamic(this, &UFistBoundTrialSubsystem::HandlePlayerDied);
	PlayerCharacter = NewCharacter;
}

void UFistBoundTrialSubsystem::SetCenterMessage(const FString& Message, float DurationSeconds)
{
	CenterMessage = Message;
	MessageExpireSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() + DurationSeconds : 0.0f;
}

FString UFistBoundTrialSubsystem::GetCenterMessage() const
{
	if (GetWorld() && GetWorld()->GetTimeSeconds() <= MessageExpireSeconds)
	{
		return CenterMessage;
	}
	return FString();
}

float UFistBoundTrialSubsystem::GetElapsedSeconds() const
{
	if (State == EFistBoundTrialState::Fighting && GetWorld())
	{
		return AccumulatedSeconds + (GetWorld()->GetTimeSeconds() - SegmentStartSeconds);
	}
	return AccumulatedSeconds;
}

void UFistBoundTrialSubsystem::PeriodicCheck()
{
	using namespace FistBoundTrial;
	UWorld* World = GetWorld();
	if (!World || State != EFistBoundTrialState::Fighting)
	{
		return;
	}

	// Engage the boss once the corridor is cleared and the player steps in.
	if (!bBossEngaged && EncountersCompleted >= Encounters.Num() && Boss && PlayerCharacter && !PlayerCharacter->IsDead())
	{
		if (FVector::Dist2D(PlayerCharacter->GetActorLocation(), Boss->GetActorLocation()) <= BossEngageDistance)
		{
			bBossEngaged = true;
			Boss->Activate();
			SetCenterMessage(TEXT("THE TRIAL WARDEN"), 3.0f);
			UE_LOG(LogFistBound, Log, TEXT("Boss engaged"));
		}
	}

	// Fall recovery: anything that slips off the floating arena comes back.
	const float FallZ = ArenaOrigin.Z - FallRecoveryDepth;
	if (PlayerCharacter && !PlayerCharacter->IsDead() && PlayerCharacter->GetActorLocation().Z < FallZ)
	{
		const FTransform& Recovery = bCheckpointReached ? CheckpointTransform : StartTransform;
		PlayerCharacter->SetActorLocation(Recovery.GetLocation());
		if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
		}
	}
	if (Boss && !Boss->IsDead() && Boss->GetActorLocation().Z < FallZ)
	{
		Boss->SetActorLocation(BossHomeTransform.GetLocation());
	}
	for (TActorIterator<AFistBoundEnemyCharacter> It(World); It; ++It)
	{
		AFistBoundEnemyCharacter* Enemy = *It;
		if (Enemy && !Enemy->IsA<AFistBoundBossCharacter>() && !Enemy->IsDead() && Enemy->GetActorLocation().Z < FallZ)
		{
			Enemy->GetHealthComponent()->ApplyDamage(9999.0f, nullptr);
		}
	}
}

void UFistBoundTrialSubsystem::HandleEncounterActivated(AFistBoundEncounter* Encounter)
{
	const int32 Index = Encounters.IndexOfByKey(Encounter);
	SetCenterMessage(FString::Printf(TEXT("ENCOUNTER %d"), Index + 1), 2.5f);
}

void UFistBoundTrialSubsystem::HandleEncounterCompleted(AFistBoundEncounter* Encounter)
{
	++EncountersCompleted;
	if (EncountersCompleted >= Encounters.Num())
	{
		if (GateWall)
		{
			GateWall->Destroy();
			GateWall = nullptr;
		}
		bCheckpointReached = true;
		SetCenterMessage(TEXT("CHECKPOINT REACHED - THE WAY OPENS"), 4.0f);
		UE_LOG(LogFistBound, Log, TEXT("All encounters complete; checkpoint reached"));
	}
	else
	{
		SetCenterMessage(TEXT("ENCOUNTER CLEARED"), 2.5f);
	}
}

void UFistBoundTrialSubsystem::HandlePlayerDied(AActor* DamageCauser)
{
	if (State != EFistBoundTrialState::Fighting)
	{
		return;
	}
	AccumulatedSeconds = GetElapsedSeconds();
	State = EFistBoundTrialState::Defeat;
	UE_LOG(LogFistBound, Log, TEXT("Trial defeat at %.2fs (checkpoint reached: %s)"), AccumulatedSeconds, bCheckpointReached ? TEXT("yes") : TEXT("no"));
}

void UFistBoundTrialSubsystem::HandleBossDied(AActor* DamageCauser)
{
	if (State != EFistBoundTrialState::Fighting)
	{
		return;
	}
	AccumulatedSeconds = GetElapsedSeconds();
	State = EFistBoundTrialState::Victory;
	UE_LOG(LogFistBound, Log, TEXT("Trial victory in %.2fs"), AccumulatedSeconds);
}

void UFistBoundTrialSubsystem::HandleRestartInput()
{
	switch (State)
	{
	case EFistBoundTrialState::Victory:
		ReloadTrial();
		break;
	case EFistBoundTrialState::Defeat:
		if (bCheckpointReached)
		{
			RetryFromCheckpoint();
		}
		else
		{
			ReloadTrial();
		}
		break;
	default:
		break;
	}
}

void UFistBoundTrialSubsystem::HandleReturnToMenuInput()
{
	if (State != EFistBoundTrialState::Victory && State != EFistBoundTrialState::Defeat)
	{
		return;
	}

	if (AFistBoundPlayerController* PC = Cast<AFistBoundPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->ReturnToMainMenu();
	}
	else
	{
		UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/GameForge/Maps/MainMenu")));
	}
}

void UFistBoundTrialSubsystem::RetryFromCheckpoint()
{
	if (Boss)
	{
		Boss->ResetForRetry();
	}
	bBossEngaged = false;
	SpawnPlayerAt(CheckpointTransform);
	State = EFistBoundTrialState::Fighting;
	SegmentStartSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	SetCenterMessage(TEXT("CHECKPOINT - AGAIN"), 2.5f);
	UE_LOG(LogFistBound, Log, TEXT("Retry from checkpoint"));
}

void UFistBoundTrialSubsystem::ReloadTrial()
{
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}
