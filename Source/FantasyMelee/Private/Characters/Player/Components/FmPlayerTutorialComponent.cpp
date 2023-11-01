// Copyright AB. All Rights Reserved.

#include "Characters/Player/Components/FmPlayerTutorialComponent.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Tags/TutorialTags.h"
#include "UI/FmHud.h"

bool UFmPlayerTutorialComponent::IsTutorialTag(const FGameplayTag& Tag)
{
	return Tag.MatchesTag(TAG_Tutorial);
}

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
}

bool UFmPlayerTutorialComponent::GrantTutorial(const FGameplayTag& TutorialTag, const AFmPlayerCharacter* Player) const
{
	if (const auto Tutorial = GetTutorialByTag(TutorialTag))
	{
		if (const auto Hud = Player->GetCustomHud())
		{
			Hud->BroadcastTutorial(*Tutorial);
		}

		return true;
	}

	return false;
}

const FFmTutorialData* UFmPlayerTutorialComponent::GetTutorialByTag(const FGameplayTag& Tag) const
{
	TArray<FFmTutorialData*> AllTutorialRows;
	TutorialDataTable->GetAllRows<FFmTutorialData>(TEXT("TutorialDataTable GetAllRows"), AllTutorialRows);
	
	TArray<FFmTutorialData> AllTutorials;

	for (const auto TutorialRow : AllTutorialRows)
	{
		AllTutorials.Add(*TutorialRow);
	}
	
	if (const auto Tutorial = AllTutorials.FindByPredicate([&Tag](const FFmTutorialData& Data)
	{
		return Tag.MatchesTagExact(Data.TagId);
	}))
	{
		return Tutorial;
	}

	return nullptr;
}
