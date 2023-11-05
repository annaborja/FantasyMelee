// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "FmBtServiceSetRandomInt.generated.h"

UCLASS()
class FANTASYMELEE_API UFmBtServiceSetRandomInt : public UBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditInstanceOnly, Category="FM Blackboard")
	FBlackboardKeySelector Selector;
	
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	int32 Min = 1;
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	int32 Max = 100;
};
