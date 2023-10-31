// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "FantasyMeleeGameModeBase.generated.h"

class UDataTable;
class UFmGasDataAsset;

UCLASS()
class FANTASYMELEE_API AFantasyMeleeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer& GetNpcCharacterTags(const FGameplayTag& TagId);
	
	FORCEINLINE UFmGasDataAsset* GetGasDataAsset() const { return GasDataAsset; }
	FORCEINLINE UDataTable* GetHitBoxDataTable() const { return HitBoxDataTable; }
	FORCEINLINE UDataTable* GetMajorNpcDataTable() const { return MajorNpcDataTable; }

private:
	UPROPERTY(EditDefaultsOnly, Category="FM Data")
	TObjectPtr<UFmGasDataAsset> GasDataAsset;
	UPROPERTY(EditDefaultsOnly, Category="FM Data", meta=(RowType="/Script/FantasyMelee.FmHitBoxData"))
	TObjectPtr<UDataTable> HitBoxDataTable;
	UPROPERTY(EditDefaultsOnly, Category="FM Data", meta=(RowType="/Script/FantasyMelee.FmMajorNpcData"))
	TObjectPtr<UDataTable> MajorNpcDataTable;

	// TODO(P0): Load from saved data.
	UPROPERTY(SaveGame, EditAnywhere, Category="FM Runtime")
	TMap<FGameplayTag, FGameplayTagContainer> NpcCharacterTagsMap;
};
