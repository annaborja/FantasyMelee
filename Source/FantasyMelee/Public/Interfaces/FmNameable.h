// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FmNameable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UFmNameable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmNameable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Data")
	virtual FText GetInGameName() const = 0;
};
