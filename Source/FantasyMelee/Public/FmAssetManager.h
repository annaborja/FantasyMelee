// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "FmAssetManager.generated.h"

UCLASS()
class FANTASYMELEE_API UFmAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UFmAssetManager& Get();
	
	virtual void StartInitialLoading() override;
};
