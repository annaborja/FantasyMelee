// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FmMotionWarpable.generated.h"

class UMotionWarpingComponent;

UINTERFACE(MinimalAPI)
class UFmMotionWarpable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmMotionWarpable
{
	GENERATED_BODY()
	
public:
	virtual UMotionWarpingComponent* GetMotionWarpingComponent() const = 0;
};
