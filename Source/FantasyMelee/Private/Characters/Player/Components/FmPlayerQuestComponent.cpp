// Copyright AB. All Rights Reserved.

#include "Characters/Player/Components/FmPlayerQuestComponent.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Tags/QuestTags.h"
#include "UI/FmHud.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

bool UFmPlayerQuestComponent::IsQuestTag(const FGameplayTag& Tag)
{
	return Tag.MatchesTag(TAG_Quest);
}

bool UFmPlayerQuestComponent::IsQuestStepTag(const FGameplayTag& Tag)
{
	return Tag.MatchesTag(TAG_QuestStep);
}

UFmPlayerQuestComponent::UFmPlayerQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFmPlayerQuestComponent::BeginPlay()
{
	Super::BeginPlay();
	
	check(QuestDataTable);
	check(QuestStepDataTable);
}

bool UFmPlayerQuestComponent::GrantQuest(const FGameplayTag& QuestTag, const AFmPlayerCharacter* Player)
{
	if (GrantedQuestsMap.Contains(QuestTag)) return false;
	
	if (const auto Quest = GetQuestByTag(QuestTag))
	{
		if (bDebugQuestGrants)
		{
			UE_LOG(LogTemp, Warning, TEXT("GrantQuest: %s"), *Quest->TagId.ToString())
		}

		auto& GrantedQuest = GrantedQuestsMap.Add(QuestTag, FFmGrantedQuest(*Quest));

		if (const auto Hud = Player->GetCustomHud())
		{
			Hud->BroadcastQuest(GrantedQuest.QuestData, QuestGranted);
		}

		// Grant the first quest step.
		if (!GrantQuestStepFromPotentialTags(&GrantedQuest, GrantedQuest.QuestData.PotentialFirstQuestStepTags, Player))
		{
			// We shouldn't ever reach this point, but account for it just in case.
			SucceedQuest(GrantedQuest, Player);
		}

		return true;
	}

	return false;
}

/**
 * Checks if any of the player's quests should be advanced.
 */
void UFmPlayerQuestComponent::OnTagSpecGrant(const AFmPlayerCharacter* Player)
{
	for (auto& GrantedQuestMapping : GrantedQuestsMap)
	{
		auto& GrantedQuest = GrantedQuestMapping.Value;
		
		if (GrantedQuest.Outcome != EFmQuestOutcome::None) continue;
		if (GrantedQuest.GrantedQuestSteps.Num() <= 0) continue;

		if (const auto LastQuestStep = GrantedQuest.GrantedQuestSteps.Last().QuestStepData;
			UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, LastQuestStep.SuccessTagRequirements) &&
			UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, LastQuestStep.SuccessTagProhibitions, true))
		{
			SucceedQuestStep(GrantedQuest, LastQuestStep, Player);
		} else if (UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, LastQuestStep.FailureTagRequirements) &&
			UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, LastQuestStep.FailureTagProhibitions, true))
		{
			FailQuestStep(GrantedQuest, LastQuestStep, Player);
		}
	}
}

const FFmQuestData* UFmPlayerQuestComponent::GetQuestByTag(const FGameplayTag& Tag) const
{
	TArray<FFmQuestData*> AllQuestRows;
	QuestDataTable->GetAllRows<FFmQuestData>(TEXT("QuestDataTable GetAllRows"), AllQuestRows);
	
	TArray<FFmQuestData> AllQuests;

	for (const auto QuestRow : AllQuestRows)
	{
		AllQuests.Add(*QuestRow);
	}
	
	if (const auto Quest = AllQuests.FindByPredicate([&Tag](const FFmQuestData& Data)
	{
		return Tag.MatchesTagExact(Data.TagId);
	}))
	{
		return Quest;
	}

	return nullptr;
}

const FFmQuestStepData* UFmPlayerQuestComponent::GetQuestStepByTag(const FGameplayTag& Tag) const
{
	TArray<FFmQuestStepData*> AllQuestStepRows;
	QuestStepDataTable->GetAllRows<FFmQuestStepData>(TEXT("QuestStepDataTable GetAllRows"), AllQuestStepRows);
	
	TArray<FFmQuestStepData> AllQuestSteps;

	for (const auto QuestStepRow : AllQuestStepRows)
	{
		AllQuestSteps.Add(*QuestStepRow);
	}
	
	if (const auto QuestStep = AllQuestSteps.FindByPredicate([&Tag](const FFmQuestStepData& Data)
	{
		return Tag.MatchesTagExact(Data.TagId);
	}))
	{
		return QuestStep;
	}

	return nullptr;
}

bool UFmPlayerQuestComponent::GrantQuestStepFromPotentialTags(FFmGrantedQuest* GrantedQuest,
	const TArray<FGameplayTag>& PotentialQuestStepTags, const AFmPlayerCharacter* Player) const
{
	FFmGrantedQuestStep GrantedQuestStep(GrantedQuest->GrantedQuestSteps.Num() + 1);
	bool bSuccess = false;

	for (const auto& RawPotentialQuestStepTag : PotentialQuestStepTags)
	{
		if (const auto QuestStep = GetQuestStepByTag(UFmBlueprintFunctionLibrary::GetNormalizedTag(RawPotentialQuestStepTag)))
		{
			if (UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, QuestStep->TagRequirements) &&
				UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, QuestStep->TagProhibitions, true))
			{
				GrantedQuestStep.QuestStepData = *QuestStep;
				bSuccess = true;
				break;
			}
		}
	}

	if (bSuccess)
	{
		if (bDebugQuestGrants)
		{
			UE_LOG(LogTemp, Warning, TEXT("GrantQuestStep: %s"), *GrantedQuestStep.QuestStepData.TagId.ToString())
		}

		GrantedQuest->GrantedQuestSteps.Add(GrantedQuestStep);

		if (const auto Hud = Player->GetCustomHud())
		{
			Hud->BroadcastQuestStep(GrantedQuestStep.QuestStepData);
		}
	}

	return bSuccess;
}

void UFmPlayerQuestComponent::SucceedQuest(FFmGrantedQuest& GrantedQuest, const AFmPlayerCharacter* Player) const
{
	GrantedQuest.Outcome = EFmQuestOutcome::Success;

	if (const auto Hud = Player->GetCustomHud())
	{
		Hud->BroadcastQuest(GrantedQuest.QuestData, QuestSucceeded);
	}
	
	UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(this, GrantedQuest.QuestData.SuccessTagGrants);
}

void UFmPlayerQuestComponent::SucceedQuestStep(FFmGrantedQuest& GrantedQuest, const FFmQuestStepData& QuestStep, const AFmPlayerCharacter* Player) const
{
	UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(this, QuestStep.SuccessTagGrants);

	// Grant the next quest step (if any).
	if (!GrantQuestStepFromPotentialTags(&GrantedQuest, QuestStep.PotentialNextQuestStepTags, Player))
	{
		SucceedQuest(GrantedQuest, Player);
	}
}

void UFmPlayerQuestComponent::FailQuestStep(FFmGrantedQuest& GrantedQuest, const FFmQuestStepData& QuestStep, const AFmPlayerCharacter* Player) const
{
	UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(this, QuestStep.FailureTagGrants);

	// Fail the quest.
	GrantedQuest.Outcome = EFmQuestOutcome::Failure;

	if (const auto Hud = Player->GetCustomHud())
	{
		Hud->BroadcastQuest(GrantedQuest.QuestData, QuestFailed);
	}
	
	UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(this, GrantedQuest.QuestData.FailureTagGrants);
}
