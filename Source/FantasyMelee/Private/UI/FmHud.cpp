// Copyright AB. All Rights Reserved.

#include "UI/FmHud.h"

#include "Characters/NPCs/FmMajorNpcCharacter.h"
#include "Characters/Player/FmPlayerCharacter.h"
#include "Characters/Player/FmPlayerController.h"
#include "Characters/Player/FmPlayerState.h"
#include "Characters/Player/Components/FmPlayerQuestComponent.h"
#include "Characters/Player/Components/FmPlayerTutorialComponent.h"
#include "Components/FmInventoryComponent.h"
#include "GAS/Attributes/FmBaseAttributeSet.h"
#include "UI/Widgets/FmDialogueOverlayWidget.h"
#include "UI/Widgets/FmPersistentOverlayWidget.h"

TMap<FGameplayTag, int32> AFmHud::GetPlayerInventory() const
{
	if (PlayerController)
	{
		if (const auto Player = PlayerController->GetPlayer())
		{
			if (const auto InventoryComponent = Player->GetInventoryComponent())
			{
				return InventoryComponent->GetInventory();
			}
		}
	}
	
	return TMap<FGameplayTag, int32>();
}

TMap<FGameplayTag, FFmGrantedQuest> AFmHud::GetPlayerQuestsMap() const
{
	if (PlayerController)
	{
		if (const auto Player = PlayerController->GetPlayer())
		{
			if (const auto QuestComponent = Player->GetPlayerQuestComponent())
			{
				return QuestComponent->GetGrantedQuestsMap();
			}
		}
	}

	return TMap<FGameplayTag, FFmGrantedQuest>();
}

void AFmHud::AdvanceDialogueStep(const AFmMajorNpcCharacter* Npc, const FFmDialogueStepData& DialogueStep) const
{
	if (!PlayerController) return;
	
	Npc->AdvanceDialogueStep(DialogueStep, PlayerController, this);
}

void AFmHud::SelectDialogueOption(const AFmMajorNpcCharacter* Npc, const FFmDialogueOptionData& DialogueOption) const
{
	if (!PlayerController) return;
	
	Npc->SelectDialogueOption(DialogueOption, PlayerController, this);
}

void AFmHud::Init(AFmPlayerController* InPlayerController)
{
	check(DialogueOverlayClass);
	check(MenuSwitcherClass);
	check(PersistentOverlayClass);

	PlayerController = InPlayerController;
	
	TArray<UFmBaseAttributeSet*> AttributeSets;

	// Bind to GAS delegates.
	if (PlayerController)
	{
		if (const auto PlayerState = PlayerController->GetPlayerState<AFmPlayerState>())
		{
			AttributeSets.Append(PlayerState->GetAttributeSets());
		
			if (const auto AbilitySystemComponent = Cast<UFmAbilitySystemComponent>(PlayerState->GetAbilitySystemComponent()))
			{
				for (const auto& AttributeSet : AttributeSets)
				{
					for (const auto& TagAttributeMapping : AttributeSet->GetTagAttributeMap())
					{
						AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TagAttributeMapping.Value())
							.AddLambda([this, AttributeSet, TagAttributeMapping](const FOnAttributeChangeData& Data)
							{
								BroadcastAttributeValue(TagAttributeMapping.Key, TagAttributeMapping.Value(), AttributeSet);
							});
					}
				}	
			}
		}
	}

	// Initialize the persistent overlay.
	if (const auto World = GetWorld(); World && PersistentOverlayClass)
	{
		PersistentOverlayWidget = CreateWidget<UFmPersistentOverlayWidget>(World, PersistentOverlayClass);
		PersistentOverlayWidget->SetCustomHud(this);

		for (const auto& AttributeSet : AttributeSets)
		{
			for (const auto& Mapping : AttributeSet->GetTagAttributeMap())
			{
				BroadcastAttributeValue(Mapping.Key, Mapping.Value(), AttributeSet);
			}
		}
		
		PersistentOverlayWidget->AddToViewport();
	}
}

void AFmHud::BroadcastQuest(const FFmQuestData& Quest, const EFmQuestBroadcastType QuestBroadcastType)
{
	QueueNotificationBroadcast([this, Quest, QuestBroadcastType]
	{
		QuestDelegate.Broadcast(Quest, QuestBroadcastType);
	});
}

void AFmHud::BroadcastQuestStep(const FFmQuestStepData& QuestStep)
{
	QueueNotificationBroadcast([this, QuestStep]
	{
		QuestStepDelegate.Broadcast(QuestStep);
	});
}

void AFmHud::BroadcastTargetInteractable(const AActor* TargetInteractable) const
{
	TargetInteractableDelegate.Broadcast(TargetInteractable);
}

void AFmHud::BroadcastTutorial(const FFmTutorialData& Tutorial) const
{
	TutorialDelegate.Broadcast(Tutorial);
}

void AFmHud::OpenMainMenu() const
{
	if (!PersistentOverlayWidget) return;
	
	PersistentOverlayWidget->OpenMainMenu();
}

void AFmHud::CloseDialogueFlow() const
{
	if (!DialogueOverlayWidget) return;
	
	DialogueOverlayWidget->DeactivateWidget();
}

void AFmHud::DisplayDialogueOptions(const AFmMajorNpcCharacter* Npc, const TArray<FFmDialogueOptionData>& DialogueOptions) const
{
	if (!DialogueOverlayWidget) return;
	
	DialogueOverlayWidget->DisplayDialogueOptions(Npc, DialogueOptions);
}

void AFmHud::DisplayDialogueStep(const AFmMajorNpcCharacter* Npc, const FFmDialogueStepData& DialogueStep) const
{
	if (!DialogueOverlayWidget) return;
	
	DialogueOverlayWidget->DisplayDialogueStep(Npc, DialogueStep);
}

void AFmHud::OpenDialogueFlow()
{
	if (!PersistentOverlayWidget) return;
	
	DialogueOverlayWidget = PersistentOverlayWidget->OpenDialogueFlow();
}

void AFmHud::OnNotificationFadeInEnd()
{
}

void AFmHud::OnNotificationFadeOutStart()
{
	bNotificationActive = false;
	ProcessNotificationQueue();
}

void AFmHud::BroadcastAttributeValue(const FGameplayTag& Tag, const FGameplayAttribute& Attribute, const UFmBaseAttributeSet* AttributeSet) const
{
	AttributeValueDelegate.Broadcast(Tag, Attribute.GetNumericValue(AttributeSet));
}

void AFmHud::ProcessNotificationQueue()
{
	if (!NotificationBroadcastQueue.IsValidIndex(0) || bNotificationActive) return;

	NotificationBroadcastQueue[0]();
	NotificationBroadcastQueue.RemoveAt(0);
	bNotificationActive = true;
}

void AFmHud::QueueNotificationBroadcast(const TFunction<void()>& Fn)
{
	NotificationBroadcastQueue.Add(Fn);
	ProcessNotificationQueue();
}
