// Copyright AB. All Rights Reserved.

#include "GAS/Abilities/FmAttackAbility.h"

#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/NotifyStates/FmHitBoxNotifyState.h"
#include "Components/FmCombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tags/TempTags.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

void UFmAttackAbility::OnHitBoxActivate(const FGameplayEventData Payload)
{
	if (const auto CombatComponent = UFmBlueprintFunctionLibrary::GetCombatComponent((GetAvatarActorFromActorInfo())))
	{
		if (const auto NotifyState = Cast<UFmHitBoxNotifyState>(Payload.OptionalObject))
		{
			CombatComponent->AddActiveHitBox(NotifyState->HitBoxDataRowHandle);
		}
	}
}

void UFmAttackAbility::OnHitBoxDeactivate(const FGameplayEventData Payload)
{
	if (const auto CombatComponent = UFmBlueprintFunctionLibrary::GetCombatComponent(GetAvatarActorFromActorInfo()))
	{
		if (const auto NotifyState = Cast<UFmHitBoxNotifyState>(Payload.OptionalObject))
		{
			CombatComponent->RemoveActiveHitBox(NotifyState->HitBoxDataRowHandle);
		}
	}
}

void UFmAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const auto WaitHitBoxActivateTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent( this, TAG_Event_Animation_HitBox_Activate);
	WaitHitBoxActivateTask->EventReceived.AddDynamic(this, &ThisClass::OnHitBoxActivate);
	WaitHitBoxActivateTask->Activate();

	const auto WaitHitBoxDeactivateTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent( this, TAG_Event_Animation_HitBox_Deactivate);
	WaitHitBoxDeactivateTask->EventReceived.AddDynamic(this, &ThisClass::OnHitBoxDeactivate);
	WaitHitBoxDeactivateTask->Activate();

	const auto AvatarActor = GetAvatarActorFromActorInfo();

	if (const auto MotionWarpingComponent = UFmBlueprintFunctionLibrary::GetMotionWarpingComponent(AvatarActor))
	{
		if (const auto CombatComponent = UFmBlueprintFunctionLibrary::GetCombatComponent(AvatarActor))
		{
			const auto AvatarActorLocation = AvatarActor->GetActorLocation();
			auto WarpTargetLocation = AvatarActorLocation + AvatarActor->GetActorForwardVector() * TranslationForward;
			
			if (const auto SoftTarget = CombatComponent->FindSoftTarget(TranslationForward))
			{
				WarpTargetLocation = SoftTarget->GetActorLocation();
			}
			
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName,
				WarpTargetLocation, UKismetMathLibrary::FindLookAtRotation(AvatarActorLocation, WarpTargetLocation));
		}
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UFmAttackAbility::OnMontageEnd()
{
	if (const auto MotionWarpingComponent = UFmBlueprintFunctionLibrary::GetMotionWarpingComponent(GetAvatarActorFromActorInfo()))
	{
		MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);
	}
	
	Super::OnMontageEnd();
}
