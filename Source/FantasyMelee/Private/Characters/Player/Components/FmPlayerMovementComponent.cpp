// Copyright AB. All Rights Reserved.

#include "Characters/Player/Components/FmPlayerMovementComponent.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/Constants.h"

UFmPlayerMovementComponent::UFmPlayerMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
	
	GravityScale = 2.f;
	JumpZVelocity = 640.f;
	MaxWalkSpeed = 600.f;
	
	bOrientRotationToMovement = true;
}

void UFmPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BaseGravityScale = GravityScale;
	BaseMaxWalkSpeed = MaxWalkSpeed;
	
	Player = CastChecked<AFmPlayerCharacter>(GetOwner());
	Player->OnReachedJumpApex.AddDynamic(this, &ThisClass::OnJumpApexReached);
}

float UFmPlayerMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Walking && !IsCrouching() && bWantsToSprint) return MaxSprintSpeed;
	
	if (MovementMode != MOVE_Custom) return Super::GetMaxSpeed();
	
	switch(CustomMovementMode)
	{
	case None:
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode %d"), CustomMovementMode)
		return 0.f;
	}
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

void UFmPlayerMovementComponent::ToggleSprint(const bool bInWantsToSprint)
{
	bWantsToSprint = bInWantsToSprint;
}

void UFmPlayerMovementComponent::OnMovementModeChanged(const EMovementMode PreviousMovementMode, const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Falling)
	{
		GravityScale = BaseGravityScale;
		
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

bool UFmPlayerMovementComponent::IsCustomMovementModeActive(const EFmCustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool UFmPlayerMovementComponent::TryMantle()
{
	// Can only mantle if we're walking/standing or jumping/falling.
	if (!(MovementMode == MOVE_Walking && !IsCrouching()) && MovementMode != MOVE_Falling) return false;
	
	if (!Player) return false;

	if (const auto Capsule = Player->GetCapsuleComponent())
	{
		const auto CapsuleHalfHeight = UKismetMathLibrary::Conv_FloatToDouble(Capsule->GetScaledCapsuleHalfHeight());
		const auto CapsuleRadius = UKismetMathLibrary::Conv_FloatToDouble(Capsule->GetScaledCapsuleRadius());
		
		const auto CapsuleBaseLocation = UpdatedComponent->GetComponentLocation() + FVector::DownVector * CapsuleHalfHeight;
		const auto ForwardNormal2d = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	
		// The valid forward distance of the mantle depends on the character's velocity.
		const auto ValidForwardDistance = FMath::Clamp(Velocity.Dot(ForwardNormal2d),
			CapsuleRadius + 30.0, UKismetMathLibrary::Conv_FloatToDouble(MantleMaxDistance));
		
		FHitResult FrontHit;
		// Start tracing at the height where the character can't just step up onto something.
		auto FrontTraceStart = CapsuleBaseLocation + FVector::UpVector * UKismetMathLibrary::Conv_FloatToDouble(MaxStepHeight - 1.f);

		for (int i = 0; i < 6; i++)
		{
			if (UKismetSystemLibrary::LineTraceSingle(this, FrontTraceStart, FrontTraceStart + ForwardNormal2d * ValidForwardDistance,
				UEngineTypes::ConvertToTraceType(TRACE_CHANNEL_CLIMBABLE), false, TArray<AActor*>(),
				bDebugMantle ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, FrontHit, true))
			{
				break;
			}
		
			FrontTraceStart += FVector::UpVector * ((2.0 * CapsuleHalfHeight - (MaxStepHeight - 1.0)) / 5.0);
		}

		// auto Params = Player->GetIgnoreCharacterParams();
		float MaxHeight = Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2+ MantleReachHeight;
		float CosMMWSA = FMath::Cos(FMath::DegreesToRadians(MantleMinWallSteepnessAngle));
		float CosMMSA = FMath::Cos(FMath::DegreesToRadians(MantleMaxSurfaceAngle));
		float CosMMAA = FMath::Cos(FMath::DegreesToRadians(MantleMaxAlignmentAngle));
		////

	
		if (!FrontHit.IsValidBlockingHit()) return false;
		float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
		if (FMath::Abs(CosWallSteepnessAngle) > CosMMWSA || (ForwardNormal2d | -FrontHit.Normal) < CosMMAA) return false;
		
		UE_LOG(LogTemp, Warning, TEXT("TryMantle - 2")) // TODO(P0): Remove log.

		// POINT(FrontHit.Location, FColor::Red);
	
		// Check Height
		TArray<FHitResult> HeightHits;
		FHitResult SurfaceHit;
		FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.Normal).GetSafeNormal();
		float WallCos = FVector::UpVector | FrontHit.Normal;
		float WallSin = FMath::Sqrt(1 - WallCos * WallCos);
		FVector TraceStart = FrontHit.Location + ForwardNormal2d + WallUp * (MaxHeight - (MaxStepHeight - 1)) / WallSin;
		// LINE(TraceStart, FrontHit.Location + ForwardNormal2d, FColor::Orange)
		if (!GetWorld()->LineTraceMultiByProfile(HeightHits, TraceStart, FrontHit.Location + ForwardNormal2d, "BlockAll"/*, Params*/)) return false;
		for (const FHitResult& Hit : HeightHits)
		{
			if (Hit.IsValidBlockingHit())
			{
				SurfaceHit = Hit;
				break;
			}
		}
		if (!SurfaceHit.IsValidBlockingHit() || (SurfaceHit.Normal | FVector::UpVector) < CosMMSA) return false;
		float Height = (SurfaceHit.Location - CapsuleBaseLocation) | FVector::UpVector;

		// SLOG(FString::Printf(TEXT("Height: %f"), Height))
		// POINT(SurfaceHit.Location, FColor::Blue);
	
		if (Height > MaxHeight) return false;
	

		// Check Clearance
		float SurfaceCos = FVector::UpVector | SurfaceHit.Normal;
		float SurfaceSin = FMath::Sqrt(1 - SurfaceCos * SurfaceCos);
		FVector ClearCapLoc = SurfaceHit.Location + ForwardNormal2d * Player->GetCapsuleComponent()->GetScaledCapsuleRadius() + FVector::UpVector * (Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 1 + Player->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2 * SurfaceSin);
		FCollisionShape CapShape = FCollisionShape::MakeCapsule(Player->GetCapsuleComponent()->GetScaledCapsuleRadius(), Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		if (GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll", CapShape /*, Params*/))
		{
			// CAPSULE(ClearCapLoc, FColor::Red)
			return false;
		}
		else
		{
			// CAPSULE(ClearCapLoc, FColor::Green)
		}
		// SLOG("Can Mantle")
	
		// Mantle Selection
		FVector ShortMantleTarget = GetMantleStartLocation(FrontHit, SurfaceHit, false);
		FVector TallMantleTarget = GetMantleStartLocation(FrontHit, SurfaceHit, true);
	
		bool bTallMantle = false;
		if (MovementMode == MOVE_Walking && Height > Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2)
			bTallMantle = true;
		else if (MovementMode == MOVE_Falling && (Velocity | FVector::UpVector) < 0)
		{
			if (!GetWorld()->OverlapAnyTestByProfile(TallMantleTarget, FQuat::Identity, "BlockAll", CapShape/*, Params*/))
				bTallMantle = true;
		}
		FVector TransitionTarget = bTallMantle ? TallMantleTarget : ShortMantleTarget;
		// CAPSULE(TransitionTarget, FColor::Yellow)

		// Perform Transition to Mantle
		// CAPSULE(UpdatedComponent->GetComponentLocation(), FColor::Red)

		float UpSpeed = Velocity | FVector::UpVector;
		float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

		TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
		TransitionRMS.Reset();
		TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
		TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;
	
		TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .25f);
		// SLOG(FString::Printf(TEXT("Duration: %f"), TransitionRMS->Duration))
		TransitionRMS->StartLocation = UpdatedComponent->GetComponentLocation();
		TransitionRMS->TargetLocation = TransitionTarget;

		// Apply Transition Root Motion Source
		Velocity = FVector::ZeroVector;
		SetMovementMode(MOVE_Flying);
		TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);
		TransitionName = TEXT("Mantle");

		// Animations
		if (bTallMantle)
		{
			TransitionQueuedMontage = TallMantleMontage;
			// Player->PlayAnimMontage(TransitionTallMantleMontage, 1 / TransitionRMS->Duration);
		}
		else
		{
			TransitionQueuedMontage = ShortMantleMontage;
			// Player->PlayAnimMontage(TransitionShortMantleMontage, 1 / TransitionRMS->Duration);
		}

		return true;
	}

	return false;
}

FVector UFmPlayerMovementComponent::GetMantleStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit,
	const bool bTallMantle) const
{
	return FVector();
}
