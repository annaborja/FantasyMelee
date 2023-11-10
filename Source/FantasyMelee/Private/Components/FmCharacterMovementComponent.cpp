// Copyright AB. All Rights Reserved.

#include "Components/FmCharacterMovementComponent.h"

float UFmCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Walking && !IsCrouching() && bWantsToSprint) return MaxSprintSpeed;
	
	if (MovementMode != MOVE_Custom) return Super::GetMaxSpeed();
	
	switch(CustomMovementMode)
	{
	case EFmCustomMovementMode::None:
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode %d"), CustomMovementMode)
		return 0.f;
	}
}

void UFmCharacterMovementComponent::ToggleSprint(const bool bInWantsToSprint)
{
	bWantsToSprint = bInWantsToSprint;
}
