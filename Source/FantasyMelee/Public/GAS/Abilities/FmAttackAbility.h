// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FmCombatAbility.h"
#include "Utils/Constants.h"
#include "FmAttackAbility.generated.h"

UCLASS()
class FANTASYMELEE_API UFmAttackAbility : public UFmCombatAbility
{
	GENERATED_BODY()

public:
	inline static FName WarpTargetName = FName(MOTION_WARP_TARGET_ATTACK);
	
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }

protected:
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float TranslationForward = 0.f;
	
	UFUNCTION()
	void OnHitBoxActivate(FGameplayEventData Payload);
	UFUNCTION()
	void OnHitBoxDeactivate(FGameplayEventData Payload);
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
		
	virtual void OnMontageEnd() override;
};
