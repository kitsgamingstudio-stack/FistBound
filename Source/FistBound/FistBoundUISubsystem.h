// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FistBoundUISubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

/**
 * Owns the foundation menu layer. The primary layout widget (authored in the
 * editor) registers its CommonUI stack here on construction; the PlayerController
 * and menu widgets then push/pop screens without holding hard references to each
 * other. Single stack is deliberate for the v0.1 shell (main menu + pause).
 */
UCLASS()
class FISTBOUND_API UFistBoundUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Called by the primary layout Blueprint once its stack widget exists. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void RegisterMenuStack(UCommonActivatableWidgetStack* InStack);

	/** Pushes a screen onto the menu stack. Returns the created widget, or null. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	UCommonActivatableWidget* PushMenu(TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/** Removes the top-most screen from the menu stack. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void PopMenu();

	/** Removes every screen from the menu stack. */
	UFUNCTION(BlueprintCallable, Category = "FistBound|UI")
	void ClearMenus();

	UFUNCTION(BlueprintPure, Category = "FistBound|UI")
	bool HasActiveMenu() const;

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UCommonActivatableWidgetStack> MenuStack;

	/** A menu requested before the stack registered; flushed on registration. */
	UPROPERTY(Transient)
	TSubclassOf<UCommonActivatableWidget> PendingMenuClass;
};
