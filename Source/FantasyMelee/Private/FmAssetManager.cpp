// Copyright AB. All Rights Reserved.

#include "FmAssetManager.h"

#include "AbilitySystemGlobals.h"

UFmAssetManager& UFmAssetManager::Get()
{
	check(GEngine);
	
	return *CastChecked<UFmAssetManager>(GEngine->AssetManager);
}

void UFmAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
