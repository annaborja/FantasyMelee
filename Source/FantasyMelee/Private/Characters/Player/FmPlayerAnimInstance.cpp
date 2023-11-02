// Copyright AB. All Rights Reserved.

#include "Characters/Player/FmPlayerAnimInstance.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Characters/Player/Components/FmPlayerMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFmPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const auto Character = Cast<AFmPlayerCharacter>(TryGetPawnOwner()))
	{
		if (const auto MovementComponent = Character->GetCharacterMovement())
		{
		}
	}
}
