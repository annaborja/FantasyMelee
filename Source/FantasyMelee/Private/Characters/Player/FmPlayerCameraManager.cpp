// Copyright AB. All Rights Reserved.

#include "Characters/Player/FmPlayerCameraManager.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Characters/Player/Components/FmPlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"

AFmPlayerCameraManager::AFmPlayerCameraManager() : APlayerCameraManager()
{
	ViewPitchMax = 65.f;
	ViewPitchMin = -70.f;
}

void AFmPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	const auto Controller = GetOwningPlayerController();
	check(Controller);
	
	Player = CastChecked<AFmPlayerCharacter>(Controller->GetCharacter());
}

void AFmPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, const float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (!Player) return;
	
	if (const auto MovementComponent = Player->GetCharacterMovement())
	{
		AdjustCameraForCrouch(OutVT, DeltaTime, MovementComponent);
	}
}

void AFmPlayerCameraManager::AdjustCameraForCrouch(FTViewTarget& OutVT, const float DeltaTime, const UCharacterMovementComponent* MovementComponent)
{
	const FVector TargetCrouchOffset(0, 0, MovementComponent->GetCrouchedHalfHeight() -
		Player->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset,
		FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.f, 1.f));

	if (MovementComponent->IsCrouching())
	{
		CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
		Offset -= TargetCrouchOffset;
	} else
	{
		CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
	}

	if (MovementComponent->IsMovingOnGround())
	{
		OutVT.POV.Location += Offset;
	}
}
