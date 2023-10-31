// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Utils/Structs.h"
#include "FmTagSpecGrantable.generated.h"

UINTERFACE(MinimalAPI)
class UFmTagSpecGrantable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmTagSpecGrantable
{
	GENERATED_BODY()
	
public:
	virtual void GrantTagSpec(const FFmTagSpec& TagSpec) = 0;
};
