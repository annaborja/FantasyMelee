// Copyright AB. All Rights Reserved.

#include "GAS/Abilities/FmHitReactionAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/FmCombatComponent.h"
#include "Interfaces/FmCombatable.h"
#include "Utils/Structs.h"

void UFmHitReactionAbility::OnMontageEnd()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

void UFmHitReactionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData)
	{
		if (const auto Combatable = Cast<IFmCombatable>(GetAvatarActorFromActorInfo()))
		{
			if (const auto CombatComponent = Combatable->GetCombatComponent())
			{
				const auto HitResult = TriggerEventData->ContextHandle.GetHitResult();
				const auto MontageData = Combatable->GetHitReactionMontageData();
				
				if (const auto Montage = MontageData.Montage; HitResult && Montage)
				{
					const auto PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
						this, NAME_None, Montage, MontageData.Rate, CombatComponent->GetHitReactionMontageSectionName(*HitResult));
					PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageEnd);
					PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnd);
					PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnd);
					PlayMontageAndWaitTask->Activate();

					return;
				}
			}
		}
	}
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}
