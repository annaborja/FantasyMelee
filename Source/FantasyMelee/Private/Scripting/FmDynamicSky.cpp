// Copyright AB. All Rights Reserved.

#include "Scripting/FmDynamicSky.h"

AFmDynamicSky::AFmDynamicSky(): AActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFmDynamicSky::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
