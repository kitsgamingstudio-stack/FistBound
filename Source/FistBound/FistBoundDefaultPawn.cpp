// Copyright Knight Integrated Technology Solutions. All Rights Reserved.

#include "FistBoundDefaultPawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

AFistBoundDefaultPawn::AFistBoundDefaultPawn()
{
	// FistBound routes input through Enhanced Input, so opt out of the
	// legacy axis/action bindings ADefaultPawn adds by default.
	bAddDefaultMovementBindings = false;
}

void AFistBoundDefaultPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFistBoundDefaultPawn::Move);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFistBoundDefaultPawn::Look);
		}
	}
}

void AFistBoundDefaultPawn::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator YawRotation(0.0, Controller->GetControlRotation().Yaw, 0.0);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, Axis.Y);
		AddMovementInput(Right, Axis.X);
	}
}

void AFistBoundDefaultPawn::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}
