// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FmAiController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class FANTASYMELEE_API AFmAiController : public AAIController
{
	GENERATED_BODY()

public:
	AFmAiController();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
