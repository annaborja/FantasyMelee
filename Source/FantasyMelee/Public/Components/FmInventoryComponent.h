// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "FmInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FFmItemData : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag IdTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYMELEE_API UFmInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static bool IsItemTag(const FGameplayTag& InTag);
	
	UFmInventoryComponent();

	bool AddItem(const FGameplayTag& IdTag, const int32 Count = 1);
	bool RemoveItem(const FGameplayTag& IdTag, const int32 Count = 1);

	FORCEINLINE TMap<FGameplayTag, int32> GetInventory() const { return InventoryMap; }

private:
	TMap<FGameplayTag, int32> InventoryMap;
};
