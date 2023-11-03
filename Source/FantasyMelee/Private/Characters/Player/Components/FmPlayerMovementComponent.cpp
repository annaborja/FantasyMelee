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
	} else if (MovementMode == MOVE_Falling)
	{
		TryMantle();
	}

	// Transition
	if (Safe_bTransitionFinished)
	{
		UE_LOG(LogTemp, Warning, TEXT("FINISHED RM"))

		if (TransitionName == "Mantle")
		{
			if (IsValid(TransitionQueuedMontage))
			{
				SetMovementMode(MOVE_Flying);
				CharacterOwner->PlayAnimMontage(TransitionQueuedMontage, TransitionQueuedMontageSpeed, MantleMontageSectionName);
				TransitionQueuedMontageSpeed = 0.f;
				TransitionQueuedMontage = nullptr;
			}
			else
			{
				SetMovementMode(MOVE_Walking);
			}
		}

		TransitionName = TEXT("");
		Safe_bTransitionFinished = false;
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UFmPlayerMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	if (!HasAnimRootMotion() && bHadAnimRootMotion && MovementMode == MOVE_Flying)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ending Anim Root Motion"))
		SetMovementMode(MOVE_Walking);
	}

	if (GetRootMotionSourceByID(TransitionRMS_ID) && GetRootMotionSourceByID(TransitionRMS_ID)->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		RemoveRootMotionSourceByID(TransitionRMS_ID);
		Safe_bTransitionFinished = true;
	}
	
	bHadAnimRootMotion = HasAnimRootMotion();
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
		const auto MaxMantleHeight = CapsuleHalfHeight * 2.0 + MantleReachHeight;
	
		// The valid forward distance of the mantle depends on the character's velocity.
		const auto ValidForwardDistance = FMath::Clamp(Velocity.Dot(ForwardNormal2d),
			CapsuleRadius + MantleMinDistanceOffset, UKismetMathLibrary::Conv_FloatToDouble(MantleMaxDistance));
		
		FHitResult FrontHit;
		// Enable mantling starting from the height at which the character can't just step up onto something.
		auto FrontTraceStart = CapsuleBaseLocation + FVector::UpVector * UKismetMathLibrary::Conv_FloatToDouble(MaxStepHeight - 1.f);

		for (int i = 0; i < NumMantleFrontTraceDivisions + 1; i++)
		{
			if (UKismetSystemLibrary::LineTraceSingle(this, FrontTraceStart, FrontTraceStart + ForwardNormal2d * ValidForwardDistance,
				UEngineTypes::ConvertToTraceType(TRACE_CHANNEL_CLIMBABLE), false, TArray<AActor*>(),
				bDebugMantle ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, FrontHit, true))
			{
				break;
			}
		
			FrontTraceStart += FVector::UpVector * ((MaxMantleHeight - (MaxStepHeight - 1.0)) / NumMantleFrontTraceDivisions);
		}

		// No valid front surface to mantle.
		if (!FrontHit.IsValidBlockingHit()) return false;
	
		const auto WallSteepnessAngle = FMath::RadiansToDegrees(FMath::Acos(FrontHit.ImpactNormal.Dot(FVector::UpVector)));
		const auto WallApproachAngle = FMath::RadiansToDegrees(FMath::Acos(ForwardNormal2d.Dot(-FrontHit.ImpactNormal)));

		if (bDebugMantle)
		{
			UE_LOG(LogTemp, Warning, TEXT("TryMantle WallSteepnessAngle: %g"), WallSteepnessAngle)
			UE_LOG(LogTemp, Warning, TEXT("TryMantle WallApproachAngle: %g"), WallApproachAngle)
		}

		// Wall or approach angle is invalid.
		if (WallSteepnessAngle < MantleMinWallSteepnessAngle || WallApproachAngle > MantleMaxAlignmentAngle) return false;
	
		FHitResult SurfaceHit;
		
		const auto WallUp = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.ImpactNormal).GetSafeNormal();
		const auto WallCos = WallUp.Dot(FVector::UpVector);

		if (const auto TraceEnd = FrontHit.ImpactPoint + ForwardNormal2d;
			!UKismetSystemLibrary::LineTraceSingle(this, TraceEnd + WallUp * (MaxMantleHeight - (FrontHit.ImpactPoint.Z - CapsuleBaseLocation.Z)) / WallCos, TraceEnd,
				UEngineTypes::ConvertToTraceType(TRACE_CHANNEL_CLIMBABLE), false, TArray<AActor*>(),
				bDebugMantle ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, SurfaceHit, true))
		{
			return false;
		}

		if (!SurfaceHit.IsValidBlockingHit()) return false;

		const auto SurfaceCos = SurfaceHit.ImpactNormal.Dot(FVector::UpVector);
		const auto MantleSurfaceAngle = FMath::RadiansToDegrees(FMath::Acos(SurfaceCos));
		const auto SurfaceHeight = (SurfaceHit.Location - CapsuleBaseLocation).Dot(FVector::UpVector);

		if (bDebugMantle)
		{
			UE_LOG(LogTemp, Warning, TEXT("TryMantle MantleSurfaceAngle: %g"), MantleSurfaceAngle)
			UE_LOG(LogTemp, Warning, TEXT("TryMantle SurfaceCos: %g"), SurfaceCos)
			UE_LOG(LogTemp, Warning, TEXT("TryMantle SurfaceHeight: %g"), SurfaceHeight)
		}
		
		if (MantleSurfaceAngle > MantleMaxSurfaceAngle) return false;
		
		if (SurfaceHeight > MaxMantleHeight) return false;

		const auto X = 180 - 90 - (90 - MantleSurfaceAngle);
		const auto CosX = FMath::Cos(FMath::DegreesToRadians(X));
		const auto Adj = CapsuleRadius;
		const auto Hyp = Adj / CosX;
		const auto Opp = FMath::Sqrt(FMath::Pow(Hyp, 2) - FMath::Pow(Adj, 2));
		const auto Foo = ForwardNormal2d.Dot(SurfaceHit.ImpactNormal);
		UE_LOG(LogTemp, Warning, TEXT("TryMantle Angle: %g , Adjacent: %g, Opposite: %g, Hyp: %g, Foo: %g"), X, Adj, Opp, Hyp, Foo)

		const auto SurfaceSin = FMath::Sin(FMath::Acos(SurfaceCos));
		// FVector ClearCapLoc = SurfaceHit.Location + ForwardNormal2d * CapsuleRadius + FVector::UpVector * (CapsuleHalfHeight + 1.0 + CapsuleRadius * 2 * FMath::Sin(X));
		FVector ClearCapLoc = SurfaceHit.Location + ForwardNormal2d * CapsuleRadius + FVector::UpVector * (CapsuleHalfHeight + 1.0 + (Foo > 0 ? -1 : 1) * Opp);
		
		FCollisionShape CapShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(Player);
		if (GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll", CapShape, CollisionQueryParams))
		{
			UKismetSystemLibrary::DrawDebugCapsule(this, ClearCapLoc, CapsuleHalfHeight, CapsuleRadius, Player->GetActorRotation(), FColor::Red, 5.f);
			return false;
		}
		else
		{
			UKismetSystemLibrary::DrawDebugCapsule(this, ClearCapLoc, CapsuleHalfHeight, CapsuleRadius, Player->GetActorRotation(), FColor::Green, 5.f);
		}

		FVector ShortMantleTarget = GetMantleStartLocation(FrontHit, SurfaceHit, CapsuleHalfHeight, CapsuleRadius, false);
		FVector TallMantleTarget = GetMantleStartLocation(FrontHit, SurfaceHit, CapsuleHalfHeight, CapsuleRadius, true);
	
		bool bTallMantle = false;
		if (MovementMode == MOVE_Walking && SurfaceHeight > CapsuleHalfHeight * 2)
			bTallMantle = true;
		else if (MovementMode == MOVE_Falling && (Velocity | FVector::UpVector) < 0)
		{
			if (!GetWorld()->OverlapAnyTestByProfile(TallMantleTarget, FQuat::Identity, "BlockAll", CapShape, CollisionQueryParams))
			{
				bTallMantle = true;
			}
		}

		FVector TransitionTarget = bTallMantle ? TallMantleTarget : ShortMantleTarget;
		UKismetSystemLibrary::DrawDebugCapsule(this, TransitionTarget, CapsuleHalfHeight, CapsuleRadius, Player->GetActorRotation(), FColor::Yellow, 5.f);

		float UpSpeed = Velocity | FVector::UpVector;
		float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

		TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
		TransitionRMS.Reset();
		TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
		TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;
	
		TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .25f);
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
			TransitionQueuedMontage = MantlesMontage;
			MantleMontageSectionName = FName("MantleTall");
			// Player->PlayAnimMontage(TransitionTallMantleMontage, 1 / TransitionRMS->Duration);
		}
		else
		{
			TransitionQueuedMontage = MantlesMontage;
			MantleMontageSectionName = FName("MantleShort");
			// Player->PlayAnimMontage(TransitionShortMantleMontage, 1 / TransitionRMS->Duration);
		}

		return true;
	}

	return false;
}

FVector UFmPlayerMovementComponent::GetMantleStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit, const float CapsuleHalfHeight, const float CapsuleRadius, const bool bTallMantle) const
{
	float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	float DownDistance = bTallMantle ? CapsuleHalfHeight * 2.f : MaxStepHeight - 1;
	FVector EdgeTangent = FVector::CrossProduct(SurfaceHit.Normal, FrontHit.Normal).GetSafeNormal();

	FVector MantleStart = SurfaceHit.Location;
	MantleStart += FrontHit.Normal.GetSafeNormal2D() * (2.f + CapsuleRadius);
	MantleStart += UpdatedComponent->GetForwardVector().GetSafeNormal2D().ProjectOnTo(EdgeTangent) * CapsuleRadius * .3f;
	MantleStart += FVector::UpVector * CapsuleHalfHeight;
	MantleStart += FVector::DownVector * DownDistance;
	MantleStart += FrontHit.Normal.GetSafeNormal2D() * CosWallSteepnessAngle * DownDistance;

	return MantleStart;
}
