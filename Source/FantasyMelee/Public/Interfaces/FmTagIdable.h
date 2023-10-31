// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FmTagIdable.generated.h"

struct FGameplayTag;

// `CannotImplementInterfaceInBlueprint` allows us to designate virtual functions as BlueprintCallable
// (see https://forums.unrealengine.com/t/how-do-i-expose-a-c-interface-to-blueprints-with-a-blueprintcallable-function/342082/2).
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UFmTagIdable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmTagIdable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual FGameplayTag GetTagId() const = 0;
};
