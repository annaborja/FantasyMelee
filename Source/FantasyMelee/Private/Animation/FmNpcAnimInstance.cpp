// Copyright AB. All Rights Reserved.

#include "Animation/FmNpcAnimInstance.h"

void UFmNpcAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const auto Npc = Cast<AFmNpcCharacter>(TryGetPawnOwner()))
	{
		EmotionalState = Npc->GetEmotionalState();
	}
}
