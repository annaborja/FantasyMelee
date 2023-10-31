// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FmGameplayAbility.h"
#include "FmHitReactionAbility.generated.h"

UCLASS()
class FANTASYMELEE_API UFmHitReactionAbility : public UFmGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	void OnMontageEnd();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
