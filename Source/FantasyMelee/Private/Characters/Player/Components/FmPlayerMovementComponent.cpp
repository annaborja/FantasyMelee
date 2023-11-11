// Copyright AB. All Rights Reserved.

#include "Characters/Player/Components/FmPlayerMovementComponent.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/Constants.h"

UFmPlayerMovementComponent::UFmPlayerMovementComponent()
{
	GravityScale = 2.f;
	
	bOrientRotationToMovement = true;
}

void UFmPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BaseGravityScale = GravityScale;
	
	Player = CastChecked<AFmPlayerCharacter>(GetOwner());
	Player->OnReachedJumpApex.AddDynamic(this, &ThisClass::OnJumpApexReached);
}

void UFmPlayerMovementComponent::UpdateCharacterStateBeforeMovement(const float DeltaSeconds)
{
	// If the player is trying to jump...
	if (bCustomPressedJump)
	{
		// Try mantling.
		if (TryMantle())
		{
			if (Player) Player->StopJumping();
		} else
		{
			// If mantling didn't work, revert to UE's default jump behavior.
			bCustomPressedJump = false;

			if (Player)
			{
				Player->bPressedJump = true;
				Player->CheckJumpInput(DeltaSeconds);
			}
		}
	} else if (MovementMode == MOVE_Falling)
	{
		TryMantle();
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UFmPlayerMovementComponent::ToggleCrouch()
{
	bWantsToCrouch = !bWantsToCrouch;
}

void UFmPlayerMovementComponent::ToggleCustomPressedJump(const bool bInCustomPressedJump)
{
	bCustomPressedJump = bInCustomPressedJump;
}

void UFmPlayerMovementComponent::OnMovementModeChanged(const EMovementMode PreviousMovementMode, const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Falling)
	{
		GravityScale = BaseGravityScale;
	}

	if (PreviousMovementMode == MOVE_Falling || PreviousMovementMode == MOVE_Flying)
	{
		if (Player && !Player->AllowedToJump())
		{
			Player->GetWorldTimerManager().SetTimer(JumpCooldownTimerHandle, this, &ThisClass::AllowJump, JumpCooldownTime);
		}
	}
}

void UFmPlayerMovementComponent::PhysCustom(const float deltaTime, const int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	
	switch (CustomMovementMode)
	{
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode %d"), CustomMovementMode)
	}
}

void UFmPlayerMovementComponent::AllowJump()
{
	if (!Player) return;

	Player->AllowJump();
}

void UFmPlayerMovementComponent::OnJumpApexReached()
{
	GravityScale = JumpFallGravityScale;
}
