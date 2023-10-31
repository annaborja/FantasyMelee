// Copyright AB. All Rights Reserved.

#include "GAS/Abilities/FmCombatAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/FmCombatComponent.h"
#include "Tags/TempTags.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

void UFmCombatAbility::OnCancelWindowBegin(FGameplayEventData Payload)
{
	if (const auto CombatComponent = UFmBlueprintFunctionLibrary::GetCombatComponent(GetAvatarActorFromActorInfo()))
	{
		CombatComponent->SetCancellableAbility(GetCurrentAbilitySpecHandle());
	}
}

void UFmCombatAbility::OnCancelWindowEnd(FGameplayEventData Payload)
{
	if (const auto CombatComponent = UFmBlueprintFunctionLibrary::GetCombatComponent(GetAvatarActorFromActorInfo()))
	{
		CombatComponent->UnsetCancellableAbility(GetCurrentAbilitySpecHandle());
	}
}

void UFmCombatAbility::OnMontageEnd()
{
	// TODO(P1): When should `bWasCancelled` be true?
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

void UFmCombatAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
									   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const auto WaitCancelWindowBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent( this, TAG_Event_Animation_CancelWindow_Begin);
	WaitCancelWindowBeginTask->EventReceived.AddDynamic(this, &ThisClass::OnCancelWindowBegin);
	WaitCancelWindowBeginTask->Activate();
	
	const auto WaitCancelWindowEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent( this, TAG_Event_Animation_CancelWindow_End);
	WaitCancelWindowEndTask->EventReceived.AddDynamic(this, &ThisClass::OnCancelWindowEnd);
	WaitCancelWindowEndTask->Activate();

	const auto PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, MontageData.Montage, MontageData.Rate, MontageData.StartSection);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageEnd);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnd);
	PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnd);
	PlayMontageAndWaitTask->Activate();
}
