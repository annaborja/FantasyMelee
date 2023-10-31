// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FmCombatable.generated.h"

struct FFmMontageData;
class UFmCombatComponent;

UINTERFACE(MinimalAPI)
class UFmCombatable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmCombatable
{
	GENERATED_BODY()

public:
	virtual UFmCombatComponent* GetCombatComponent() const = 0;
	virtual uint8 GetCombatLevel() const { return 1; }
	virtual FFmMontageData GetHitReactionMontageData() const = 0;
};
