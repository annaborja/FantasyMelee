// Copyright AB. All Rights Reserved.

#include "Animation/NotifyStates/FmCancelWindowNotifyState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Tags/TempTags.h"

void UFmCancelWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (const auto Owner = MeshComp->GetOwner())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, TAG_Event_Animation_CancelWindow_Begin, FGameplayEventData());
	}
}

void UFmCancelWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (const auto Owner = MeshComp->GetOwner())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, TAG_Event_Animation_CancelWindow_End, FGameplayEventData());
	}
}
