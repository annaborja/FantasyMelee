// Copyright AB. All Rights Reserved.

#include "Animation/NotifyStates/FmHitBoxNotifyState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Tags/TempTags.h"

void UFmHitBoxNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (const auto Owner = MeshComp->GetOwner())
	{
		FGameplayEventData EventData;
		EventData.OptionalObject = this;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, TAG_Event_Animation_HitBox_Activate, EventData);
	}
}

void UFmHitBoxNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (const auto Owner = MeshComp->GetOwner())
	{
		FGameplayEventData EventData;
		EventData.OptionalObject = this;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, TAG_Event_Animation_HitBox_Deactivate, EventData);
	}
}
