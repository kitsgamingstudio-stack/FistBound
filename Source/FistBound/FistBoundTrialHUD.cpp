// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundTrialHUD.h"
#include "FistBoundBossCharacter.h"
#include "FistBoundHealthComponent.h"
#include "FistBoundPlayerCharacter.h"
#include "FistBoundTrialSubsystem.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"

void AFistBoundTrialHUD::DrawHUD()
{
	Super::DrawHUD();

	UWorld* World = GetWorld();
	UFistBoundTrialSubsystem* Trial = World ? World->GetSubsystem<UFistBoundTrialSubsystem>() : nullptr;
	if (!Canvas || !Trial || Trial->GetState() == EFistBoundTrialState::Inactive)
	{
		return;
	}

	const UFont* SmallFont = GEngine->GetSmallFont();
	const UFont* MediumFont = GEngine->GetMediumFont();
	const UFont* LargeFont = GEngine->GetLargeFont();
	const float SizeX = Canvas->SizeX;
	const float SizeY = Canvas->SizeY;
	const EFistBoundTrialState State = Trial->GetState();

	// --- Timer (top center) --------------------------------------------
	DrawCenteredText(MediumFont, FormatTime(Trial->GetElapsedSeconds()), SizeX * 0.5f, SizeY * 0.03f, FLinearColor::White, 1.4f);

	// --- Encounter progress (top left) ---------------------------------
	Canvas->SetDrawColor(FColor::White);
	FString Progress = FString::Printf(TEXT("ENCOUNTERS %d/%d"), Trial->GetEncountersCompleted(), Trial->GetTotalEncounters());
	if (Trial->IsCheckpointReached())
	{
		Progress += TEXT("   CHECKPOINT REACHED");
	}
	Canvas->DrawText(MediumFont, Progress, SizeX * 0.03f, SizeY * 0.03f);

	// --- Player health (bottom left) -----------------------------------
	if (const AFistBoundPlayerCharacter* Player = Trial->GetPlayerCharacter())
	{
		if (const UFistBoundHealthComponent* Health = Player->GetHealthComponent())
		{
			const float BarX = SizeX * 0.03f;
			const float BarY = SizeY * 0.92f;
			Canvas->SetDrawColor(FColor::White);
			Canvas->DrawText(SmallFont, TEXT("HEALTH"), BarX, BarY - 18.0f);
			DrawBar(BarX, BarY, SizeX * 0.22f, 20.0f, Health->GetHealthFraction(), FLinearColor(0.15f, 0.75f, 0.25f));
		}
	}

	// --- Boss bar (upper center, while engaged) ------------------------
	const AFistBoundBossCharacter* Boss = Trial->GetBoss();
	if (Trial->IsBossEngaged() && Boss)
	{
		if (const UFistBoundHealthComponent* BossHealth = Boss->GetHealthComponent())
		{
			const float BarWidth = SizeX * 0.4f;
			const float BarX = (SizeX - BarWidth) * 0.5f;
			const float BarY = SizeY * 0.1f;
			DrawCenteredText(MediumFont, TEXT("THE TRIAL WARDEN"), SizeX * 0.5f, BarY - 24.0f, FLinearColor(1.0f, 0.75f, 0.3f), 1.0f);
			DrawBar(BarX, BarY, BarWidth, 22.0f, BossHealth->GetHealthFraction(), FLinearColor(0.8f, 0.12f, 0.1f));
		}
	}

	// --- Center banner --------------------------------------------------
	const FString Message = Trial->GetCenterMessage();
	if (!Message.IsEmpty() && State == EFistBoundTrialState::Fighting)
	{
		DrawCenteredText(LargeFont, Message, SizeX * 0.5f, SizeY * 0.28f, FLinearColor(1.0f, 0.9f, 0.5f), 1.6f);
	}

	// --- Control hints for the opening seconds --------------------------
	if (State == EFistBoundTrialState::Fighting && Trial->GetElapsedSeconds() < 12.0f)
	{
		DrawCenteredText(SmallFont, TEXT("WASD move   Mouse look   LMB light   RMB heavy   Space/Shift dodge"), SizeX * 0.5f, SizeY * 0.86f, FLinearColor(0.85f, 0.85f, 0.85f), 1.0f);
	}

	// --- Result overlay --------------------------------------------------
	if (State == EFistBoundTrialState::Victory || State == EFistBoundTrialState::Defeat)
	{
		FCanvasTileItem Overlay(FVector2D(0.0f, 0.0f), FVector2D(SizeX, SizeY), FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));
		Overlay.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Overlay);

		const bool bVictory = State == EFistBoundTrialState::Victory;
		DrawCenteredText(LargeFont, bVictory ? TEXT("VICTORY") : TEXT("DEFEATED"), SizeX * 0.5f, SizeY * 0.32f,
			bVictory ? FLinearColor(1.0f, 0.85f, 0.25f) : FLinearColor(0.9f, 0.2f, 0.15f), 3.0f);
		DrawCenteredText(MediumFont, FString::Printf(TEXT("TIME  %s"), *FormatTime(Trial->GetElapsedSeconds())), SizeX * 0.5f, SizeY * 0.45f, FLinearColor::White, 1.5f);

		const FString RestartHint = bVictory
			? TEXT("[R] RESTART TRIAL      [M] MAIN MENU")
			: (Trial->IsCheckpointReached() ? TEXT("[R] RETRY FROM CHECKPOINT      [M] MAIN MENU") : TEXT("[R] RESTART TRIAL      [M] MAIN MENU"));
		DrawCenteredText(MediumFont, RestartHint, SizeX * 0.5f, SizeY * 0.55f, FLinearColor(0.85f, 0.85f, 0.85f), 1.1f);
	}
}

void AFistBoundTrialHUD::DrawBar(float X, float Y, float Width, float Height, float Fraction, const FLinearColor& FillColor)
{
	FCanvasTileItem Background(FVector2D(X, Y), FVector2D(Width, Height), FLinearColor(0.05f, 0.05f, 0.05f, 0.8f));
	Background.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Background);

	const float ClampedFraction = FMath::Clamp(Fraction, 0.0f, 1.0f);
	if (ClampedFraction > 0.0f)
	{
		FCanvasTileItem Fill(FVector2D(X + 2.0f, Y + 2.0f), FVector2D((Width - 4.0f) * ClampedFraction, Height - 4.0f), FillColor);
		Fill.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Fill);
	}
}

void AFistBoundTrialHUD::DrawCenteredText(const UFont* Font, const FString& Text, float CenterX, float Y, const FLinearColor& Color, float Scale)
{
	float TextWidth = 0.0f;
	float TextHeight = 0.0f;
	Canvas->TextSize(Font, Text, TextWidth, TextHeight, Scale, Scale);
	Canvas->SetDrawColor(Color.ToFColor(true));
	Canvas->DrawText(Font, Text, CenterX - TextWidth * 0.5f, Y, Scale, Scale);
}

FString AFistBoundTrialHUD::FormatTime(float Seconds)
{
	const int32 Minutes = FMath::FloorToInt(Seconds / 60.0f);
	const float Remainder = Seconds - Minutes * 60.0f;
	return FString::Printf(TEXT("%02d:%05.2f"), Minutes, Remainder);
}
