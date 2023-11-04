// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FmDynamicSky.generated.h"

UCLASS()
class FANTASYMELEE_API AFmDynamicSky : public AActor
{
	GENERATED_BODY()
	
public:	
	AFmDynamicSky();
	
	virtual void OnConstruction(const FTransform& Transform) override;
};
