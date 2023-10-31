// Copyright AB. All Rights Reserved.

#include "AI/Tasks/FmBtTask_ExecuteDialogue.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/NPCs/FmMajorNpcCharacter.h"
#include "Characters/Player/FmPlayerCharacter.h"
#include "Components/FmDialogueComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/FmHud.h"

EBTNodeResult::Type UFmBtTask_ExecuteDialogue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const auto AiController = OwnerComp.GetAIOwner())
	{
		if (const auto Npc = Cast<AFmMajorNpcCharacter>(AiController->GetPawn()))
		{
			if (const auto Player = Cast<AFmPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
			{
				if (const auto DialogueComponent = Npc->GetDialogueComponent(); DialogueComponent && DialogueComponent->HasAvailableDialogue(Player))
				{
					if (const auto Hud = Player->GetCustomHud()) {
						if (!bDelegateAdded)
						{
							DialogueComponent->DialogueEndedDelegate.AddDynamic(this, &ThisClass::OnDialogueEnded);
							bDelegateAdded = true;
						}
						
						DialogueComponent->StartDialogue(Player->GetCustomController(), Hud);
					}
				}
			}
		}
	}
	
	return EBTNodeResult::InProgress;
}

void UFmBtTask_ExecuteDialogue::OnDialogueEnded(AAIController* AiController)
{
	if (const auto BlackboardComponent = AiController->GetBlackboardComponent())
	{
		if (const auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BlackboardComponent->GetBrainComponent()))
		{
			FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
		}
	}
}
