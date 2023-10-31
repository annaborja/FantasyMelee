// Copyright AB. All Rights Reserved.

#include "Components/FmInventoryComponent.h"

#include "Tags/ItemTags.h"

bool UFmInventoryComponent::IsItemTag(const FGameplayTag& InTag)
{
	return InTag.MatchesTag(TAG_Item);
}

UFmInventoryComponent::UFmInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UFmInventoryComponent::AddItem(const FGameplayTag& IdTag, const int32 Count)
{
	if (const auto ExistingValue = InventoryMap.Find(IdTag))
	{
		InventoryMap.Add(IdTag, *ExistingValue + Count);
	} else
	{
		InventoryMap.Add(IdTag, Count);
	}

	return true;
}

bool UFmInventoryComponent::RemoveItem(const FGameplayTag& IdTag, const int32 Count)
{
	// TODO(P0): Implement.
	return true;
}
