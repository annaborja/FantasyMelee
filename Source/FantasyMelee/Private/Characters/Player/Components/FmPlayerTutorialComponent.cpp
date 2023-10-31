// Copyright AB. All Rights Reserved.

#include "Characters/Player/Components/FmPlayerTutorialComponent.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "UI/FmHud.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

UFmPlayerTutorialComponent::UFmPlayerTutorialComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFmPlayerTutorialComponent::BeginPlay()
{
	Super::BeginPlay();
	
	check(TutorialDataTable);
		
	TArray<FFmTutorialData*> AllTutorialRows;
	TutorialDataTable->GetAllRows<FFmTutorialData>(TEXT("TutorialDataTable GetAllRows"), AllTutorialRows);

	IncompleteTutorials.Reset();

	for (const auto TutorialRow : AllTutorialRows.FilterByPredicate([this](const FFmTutorialData* Data)
	{
		return !CompletedTutorialTags.HasTagExact(Data->TagId);
	}))
	{
		IncompleteTutorials.Add(*TutorialRow);
	}
}

/**
 * Checks if any tutorials should be granted based on the player's owned tags.
 */
void UFmPlayerTutorialComponent::OnTagSpecGrant(const AFmPlayerCharacter* Player)
{
	if (IncompleteTutorials.Num() <= 0) return;
	
	if (const auto Hud = Player->GetCustomHud())
	{
		if (const auto TutorialIdx = IncompleteTutorials.IndexOfByPredicate([this](const FFmTutorialData& Data)
		{
			return UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, Data.TagRequirements);
		}); IncompleteTutorials.IsValidIndex(TutorialIdx))
		{
			const auto Tutorial = IncompleteTutorials[TutorialIdx];
			
			IncompleteTutorials.RemoveAt(TutorialIdx);
			CompletedTutorialTags.AddTag(Tutorial.TagId);
			Hud->BroadcastTutorial(Tutorial);
		}
	}
}
