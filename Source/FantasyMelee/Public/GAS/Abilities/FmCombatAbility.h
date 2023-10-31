// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FmGameplayAbility.h"
#include "Utils/Structs.h"
#include "FmCombatAbility.generated.h"

class IFmCombatable;

UCLASS()
class FANTASYMELEE_API UFmCombatAbility : public UFmGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	FFmMontageData MontageData;

	UFUNCTION()
	virtual void OnCancelWindowBegin(FGameplayEventData Payload);
	UFUNCTION()
	virtual void OnCancelWindowEnd(FGameplayEventData Payload);
	UFUNCTION()
	virtual void OnMontageEnd();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
