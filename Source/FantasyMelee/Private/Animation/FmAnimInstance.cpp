// Copyright AB. All Rights Reserved.

#include "Animation/FmAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UFmAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const auto Character = Cast<ACharacter>(TryGetPawnOwner()))
	{
		if (const auto MovementComponent = Character->GetCharacterMovement())
		{
			GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
			MovementDirection = UKismetAnimationLibrary::CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
			VerticalSpeed = MovementComponent->Velocity.Z;
			bIsCrouched = Character->bIsCrouched;
			bIsFalling = MovementComponent->IsFalling();
			bIsMoving = GroundSpeed > 100.f;
		}
	}
}
