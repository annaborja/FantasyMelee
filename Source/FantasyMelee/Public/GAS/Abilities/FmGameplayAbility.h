// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Interfaces/FmTagIdable.h"
#include "FmGameplayAbility.generated.h"

UCLASS()
class FANTASYMELEE_API UFmGameplayAbility : public UGameplayAbility, public IFmTagIdable
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetTagId() const override { return TagId; }

private:
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTag TagId;
};
