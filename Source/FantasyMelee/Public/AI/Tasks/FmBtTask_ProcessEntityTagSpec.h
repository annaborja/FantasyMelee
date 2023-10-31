// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Utils/Structs.h"
#include "FmBtTask_ProcessEntityTagSpec.generated.h"

UCLASS()
class FANTASYMELEE_API UFmBtService_ProcessEntityTagSpec : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category="FM Params")
	FFmEntityTagSpec TagGrants;
};
