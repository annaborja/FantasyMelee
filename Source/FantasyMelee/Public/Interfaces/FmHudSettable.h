// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FmHudSettable.generated.h"

class AFmHud;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UFmHudSettable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmHudSettable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="HUD")
	virtual void SetCustomHud(AFmHud* InCustomHud) = 0;
};
