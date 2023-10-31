// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FmInventoriable.generated.h"

class UFmInventoryComponent;

UINTERFACE(MinimalAPI)
class UFmInventoriable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmInventoriable
{
	GENERATED_BODY()

public:
	virtual UFmInventoryComponent* GetInventoryComponent() const = 0;
};
