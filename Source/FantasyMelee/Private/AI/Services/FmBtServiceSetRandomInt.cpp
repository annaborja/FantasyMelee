// Copyright AB. All Rights Reserved.

#include "AI/Services/FmBtServiceSetRandomInt.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UFmBtServiceSetRandomInt::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (const auto BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsInt(Selector.SelectedKeyName, UKismetMathLibrary::RandomIntegerInRange(Min, Max));
	}
}
