// Copyright Epic Games, Inc. All Rights Reserved.

#include "FantasyMeleeGameModeBase.h"

FGameplayTagContainer& AFantasyMeleeGameModeBase::GetNpcCharacterTags(const FGameplayTag& TagId)
{
	return NpcCharacterTagsMap.FindOrAdd(TagId);
}
