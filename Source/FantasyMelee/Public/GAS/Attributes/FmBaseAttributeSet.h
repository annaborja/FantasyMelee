// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Utils/Types.h"
#include "FmBaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

struct FGameplayTag;

UCLASS()
class FANTASYMELEE_API UFmBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	FORCEINLINE TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> GetTagAttributeMap() const { return TagAttributeMap; }

protected:
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagAttributeMap;
};
