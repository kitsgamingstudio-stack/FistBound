// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "../FistBoundHealthComponent.h"
#include "FistBoundTestHelpers.h"

// The health component is deliberately world-independent so the damage
// contract every combatant relies on can be verified without spawning a map.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFistBoundHealthDamageTest,
	"FistBound.Combat.Health.DamageAndDeath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FFistBoundHealthDamageTest::RunTest(const FString& Parameters)
{
	UFistBoundHealthComponent* Health = NewObject<UFistBoundHealthComponent>();
	Health->InitHealth(100.0f);

	TestEqual(TEXT("Starts at full health"), Health->GetHealth(), 100.0f);
	TestTrue(TEXT("Starts alive"), Health->IsAlive());

	const float Applied = Health->ApplyDamage(30.0f, nullptr);
	TestEqual(TEXT("Damage is applied in full"), Applied, 30.0f);
	TestEqual(TEXT("Health drops by the applied amount"), Health->GetHealth(), 70.0f);
	TestTrue(TEXT("Still alive above zero"), Health->IsAlive());

	// Overkill clamps to the remaining pool and kills exactly once.
	UFistBoundTestDeathListener* Listener = NewObject<UFistBoundTestDeathListener>();
	Health->OnDied.AddDynamic(Listener, &UFistBoundTestDeathListener::HandleDied);
	const float Overkill = Health->ApplyDamage(500.0f, nullptr);
	TestEqual(TEXT("Overkill clamps to remaining health"), Overkill, 70.0f);
	TestEqual(TEXT("Health floors at zero"), Health->GetHealth(), 0.0f);
	TestFalse(TEXT("Dead after lethal damage"), Health->IsAlive());
	TestEqual(TEXT("OnDied fired exactly once"), Listener->DeathCount, 1);

	// The dead take no further damage and never die twice.
	TestEqual(TEXT("Dead targets take no damage"), Health->ApplyDamage(10.0f, nullptr), 0.0f);
	TestEqual(TEXT("OnDied did not fire again"), Listener->DeathCount, 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFistBoundHealthInvulnerabilityTest,
	"FistBound.Combat.Health.InvulnerabilityWindow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FFistBoundHealthInvulnerabilityTest::RunTest(const FString& Parameters)
{
	UFistBoundHealthComponent* Health = NewObject<UFistBoundHealthComponent>();
	Health->InitHealth(100.0f);

	// Dodge i-frames: no damage lands while invulnerable, damage resumes after.
	Health->SetInvulnerable(true);
	TestEqual(TEXT("Invulnerable targets take no damage"), Health->ApplyDamage(40.0f, nullptr), 0.0f);
	TestEqual(TEXT("Health unchanged while invulnerable"), Health->GetHealth(), 100.0f);

	Health->SetInvulnerable(false);
	TestEqual(TEXT("Damage lands after the window ends"), Health->ApplyDamage(40.0f, nullptr), 40.0f);
	TestEqual(TEXT("Health reflects post-window damage"), Health->GetHealth(), 60.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFistBoundHealthResetTest,
	"FistBound.Combat.Health.ResetForRetry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FFistBoundHealthResetTest::RunTest(const FString& Parameters)
{
	UFistBoundHealthComponent* Health = NewObject<UFistBoundHealthComponent>();
	Health->InitHealth(50.0f);
	Health->ApplyDamage(50.0f, nullptr);
	TestFalse(TEXT("Dead before reset"), Health->IsAlive());

	// Checkpoint retry path: InitHealth must fully revive (boss reset).
	Health->InitHealth(220.0f);
	TestTrue(TEXT("Alive after reset"), Health->IsAlive());
	TestEqual(TEXT("Reset restores the new maximum"), Health->GetHealth(), 220.0f);
	TestEqual(TEXT("Fraction is full after reset"), Health->GetHealthFraction(), 1.0f);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
