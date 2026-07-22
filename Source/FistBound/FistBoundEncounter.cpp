// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundEncounter.h"
#include "FistBound.h"
#include "FistBoundEnemyCharacter.h"
#include "FistBoundHealthComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AFistBoundEncounter::AFistBoundEncounter()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AFistBoundEncounter::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(ProximityTimer, this, &AFistBoundEncounter::CheckActivation, 0.2f, true);
}

void AFistBoundEncounter::InitEncounter(TSubclassOf<AFistBoundEnemyCharacter> InEnemyClass, const TArray<FVector>& SpawnOffsets)
{
	UWorld* World = GetWorld();
	if (!World || !InEnemyClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FVector& Offset : SpawnOffsets)
	{
		const FVector Location = GetActorLocation() + Offset;
		// Spawn facing back down the corridor toward the approaching player.
		const FRotator Facing(0.0f, 180.0f, 0.0f);
		if (AFistBoundEnemyCharacter* Enemy = World->SpawnActor<AFistBoundEnemyCharacter>(InEnemyClass, Location, Facing, SpawnParams))
		{
			Enemy->GetHealthComponent()->OnDied.AddDynamic(this, &AFistBoundEncounter::HandleEnemyDied);
			Enemies.Add(Enemy);
			++AliveCount;
		}
	}

	UE_LOG(LogFistBound, Log, TEXT("Encounter %s spawned %d enemies"), *GetName(), AliveCount);
}

void AFistBoundEncounter::CheckActivation()
{
	if (State != EFistBoundEncounterState::Dormant)
	{
		GetWorldTimerManager().ClearTimer(ProximityTimer);
		return;
	}

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return;
	}

	if (FVector::DistSquared2D(PlayerPawn->GetActorLocation(), GetActorLocation()) <= FMath::Square(ActivationRadius))
	{
		State = EFistBoundEncounterState::Active;
		GetWorldTimerManager().ClearTimer(ProximityTimer);
		for (AFistBoundEnemyCharacter* Enemy : Enemies)
		{
			if (Enemy)
			{
				Enemy->Activate();
			}
		}
		OnActivated.Broadcast(this);
		UE_LOG(LogFistBound, Log, TEXT("Encounter %s activated"), *GetName());
	}
}

void AFistBoundEncounter::HandleEnemyDied(AActor* DamageCauser)
{
	AliveCount = FMath::Max(0, AliveCount - 1);
	if (AliveCount == 0 && State != EFistBoundEncounterState::Completed)
	{
		State = EFistBoundEncounterState::Completed;
		OnCompleted.Broadcast(this);
		UE_LOG(LogFistBound, Log, TEXT("Encounter %s completed"), *GetName());
	}
}
