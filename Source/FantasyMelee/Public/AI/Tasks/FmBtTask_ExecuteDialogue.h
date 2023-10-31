// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FmBtTask_ExecuteDialogue.generated.h"

UCLASS()
class FANTASYMELEE_API UFmBtTask_ExecuteDialogue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UFUNCTION()
	void OnDialogueEnded(AAIController* AiController);

	bool bDelegateAdded = false;
};
