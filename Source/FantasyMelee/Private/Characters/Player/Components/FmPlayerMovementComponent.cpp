// Copyright AB. All Rights Reserved.

#include "Characters/Player/Components/FmPlayerMovementComponent.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Components/CapsuleComponent.h"

UFmPlayerMovementComponent::UFmPlayerMovementComponent()
{
	GravityScale = BaseGravityScale;
	bOrientRotationToMovement = true;
	JumpZVelocity = 640.f;
	
	NavAgentProps.bCanCrouch = true;
}

void UFmPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BaseMaxWalkSpeed = MaxWalkSpeed;
	GravityScale = BaseGravityScale;

	if (PlayerOwner)
	{
		PlayerOwner->OnReachedJumpApex.AddDynamic(this, &ThisClass::OnJumpApexReached);
	}
}

bool UFmPlayerMovementComponent::IsCustomMovementModeActive(const EFmCustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UFmPlayerMovementComponent::ToggleCrouch()
{
	bWantsToCrouch = !bWantsToCrouch;
}

void UFmPlayerMovementComponent::ToggleSprint(const bool bInWantsToSprint)
{
	bWantsToSprint = bInWantsToSprint;
}

bool UFmPlayerMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

void UFmPlayerMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	PlayerOwner = Cast<AFmPlayerCharacter>(GetOwner());
}

void UFmPlayerMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Falling)
	{
		if (PlayerOwner)
		{
			GravityScale = BaseGravityScale;
			PlayerOwner->GetWorldTimerManager().SetTimer(CanJumpResetTimerHandle, this, &ThisClass::EnableJump, JumpCooldownTime);
		}
	}
}

bool UFmPlayerMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementModeActive(EFmCustomMovementMode::Slide);
}

void UFmPlayerMovementComponent::OnMovementUpdated(const float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (bWantsToSprint)
		{
			MaxWalkSpeed = BaseMaxWalkSpeed * SprintSpeedMultiplier;
		} else
		{
			MaxWalkSpeed = BaseMaxWalkSpeed;
		}
	}
}

void UFmPlayerMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	
	switch (CustomMovementMode)
	{
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode"))
	}
}

void UFmPlayerMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UFmPlayerMovementComponent::EnableJump()
{
	if (!PlayerOwner) return;

	PlayerOwner->EnableJump();
}

void UFmPlayerMovementComponent::OnJumpApexReached()
{
	GravityScale = JumpFallGravityScale;
}
