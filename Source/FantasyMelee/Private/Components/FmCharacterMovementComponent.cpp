// Copyright AB. All Rights Reserved.

#include "Components/FmCharacterMovementComponent.h"

#include "Characters/FmCharacter.h"
#include "Characters/NPCs/FmNpcCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/Constants.h"
#include "Utils/Macros.h"

UFmCharacterMovementComponent::UFmCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
	
	JumpZVelocity = 640.f;
	MaxWalkSpeed = 600.f;
}

void UFmCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Character = CastChecked<AFmCharacter>(GetOwner());
	Npc = Cast<AFmNpcCharacter>(GetOwner());
}

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

void UFmCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
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

void UFmCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	if (!HasAnimRootMotion() && bHadAnimRootMotion && MovementMode == MOVE_Flying)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ending Anim Root Motion"))
		SetMovementMode(MOVE_Walking);

		if (Npc && Npc->HasTargetMoveLocation() && Npc->GetTargetMoveLocation() != Npc->GetActorLocation())
		{
			Npc->LaunchCharacter(Npc->GetTargetMoveLocation() - Npc->GetActorLocation(), true, false);
		}
	}

	if (GetRootMotionSourceByID(TransitionRMS_ID) && GetRootMotionSourceByID(TransitionRMS_ID)->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		RemoveRootMotionSourceByID(TransitionRMS_ID);
		Safe_bTransitionFinished = true;
	}
	
	bHadAnimRootMotion = HasAnimRootMotion();
}

void UFmCharacterMovementComponent::ToggleSprint(const bool bInWantsToSprint)
{
	bWantsToSprint = bInWantsToSprint;
}

bool UFmCharacterMovementComponent::IsCustomMovementModeActive(const EFmCustomMovementMode::Type InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool UFmCharacterMovementComponent::TryMantle()
{
	// Can only mantle if we're walking/standing or jumping/falling.
	if (!(MovementMode == MOVE_Walking && !IsCrouching()) && MovementMode != MOVE_Falling) return false;
	
	if (!Character) return false;

	if (const auto Capsule = Character->GetCapsuleComponent())
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
		CollisionQueryParams.AddIgnoredActor(Character);
		if (GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll", CapShape, CollisionQueryParams))
		{
			UKismetSystemLibrary::DrawDebugCapsule(this, ClearCapLoc, CapsuleHalfHeight, CapsuleRadius, Character->GetActorRotation(), FColor::Red, 5.f);
			return false;
		}
		else
		{
			UKismetSystemLibrary::DrawDebugCapsule(this, ClearCapLoc, CapsuleHalfHeight, CapsuleRadius, Character->GetActorRotation(), FColor::Green, 5.f);
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
		UKismetSystemLibrary::DrawDebugCapsule(this, TransitionTarget, CapsuleHalfHeight, CapsuleRadius, Character->GetActorRotation(), FColor::Yellow, 5.f);

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
			// Character->PlayAnimMontage(TransitionTallMantleMontage, 1 / TransitionRMS->Duration);
		}
		else
		{
			TransitionQueuedMontage = MantlesMontage;
			MantleMontageSectionName = FName("MantleShort");
			// Character->PlayAnimMontage(TransitionShortMantleMontage, 1 / TransitionRMS->Duration);
		}

		return true;
	}

	return false;
}

FVector UFmCharacterMovementComponent::GetMantleStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit, const float CapsuleHalfHeight, const float CapsuleRadius, const bool bTallMantle) const
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

