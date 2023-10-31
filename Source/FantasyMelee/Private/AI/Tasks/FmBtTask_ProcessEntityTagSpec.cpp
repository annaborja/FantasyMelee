// Copyright AB. All Rights Reserved.

#include "AI/Tasks/FmBtTask_ProcessEntityTagSpec.h"

#include "Utils/FmBlueprintFunctionLibrary.h"

EBTNodeResult::Type UFmBtService_ProcessEntityTagSpec::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(this, TagGrants);
	
	return EBTNodeResult::Succeeded;
}
