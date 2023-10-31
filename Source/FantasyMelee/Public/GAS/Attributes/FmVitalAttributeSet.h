// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FmBaseAttributeSet.h"
#include "FmVitalAttributeSet.generated.h"

UCLASS()
class FANTASYMELEE_API UFmVitalAttributeSet : public UFmBaseAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UFmVitalAttributeSet, Health)
	
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UFmVitalAttributeSet, MaxHealth)

	UFmVitalAttributeSet();

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
