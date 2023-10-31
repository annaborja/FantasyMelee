// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FmBaseAttributeSet.h"
#include "FmPrimaryAttributeSet.generated.h"

UCLASS()
class FANTASYMELEE_API UFmPrimaryAttributeSet : public UFmBaseAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Creativity;
	ATTRIBUTE_ACCESSORS(UFmPrimaryAttributeSet, Creativity)
	
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(UFmPrimaryAttributeSet, Dexterity)
	
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UFmPrimaryAttributeSet, Intelligence)
	
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UFmPrimaryAttributeSet, Resilience)
	
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UFmPrimaryAttributeSet, Speed)
	
	UPROPERTY(VisibleAnywhere)
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UFmPrimaryAttributeSet, Strength)

	UFmPrimaryAttributeSet();
};
