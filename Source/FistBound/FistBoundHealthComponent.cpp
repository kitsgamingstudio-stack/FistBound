// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundHealthComponent.h"

UFistBoundHealthComponent::UFistBoundHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
}

void UFistBoundHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	InitHealth(MaxHealth);
}

void UFistBoundHealthComponent::InitHealth(float InMaxHealth)
{
	MaxHealth = FMath::Max(1.0f, InMaxHealth);
	CurrentHealth = MaxHealth;
	bDead = false;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float UFistBoundHealthComponent::ApplyDamage(float Amount, AActor* DamageCauser)
{
	if (bDead || bInvulnerable || Amount <= 0.0f)
	{
		return 0.0f;
	}

	const float Applied = FMath::Min(Amount, CurrentHealth);
	CurrentHealth -= Applied;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDamaged.Broadcast(Applied, DamageCauser);

	if (CurrentHealth <= 0.0f)
	{
		bDead = true;
		OnDied.Broadcast(DamageCauser);
	}
	return Applied;
}
