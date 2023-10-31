// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FmBtTask_FindTarget.generated.h"

UENUM()
enum class EFmBttFindTargetSelectionStrategy : uint8
{
	ClosestDistance,
	Random
};

UCLASS()
class FANTASYMELEE_API UFmBtTask_FindTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditInstanceOnly, Category="FM Blackboard")
	FBlackboardKeySelector TargetSelector;
	
	UPROPERTY(EditInstanceOnly, Category="FM Debug")
	bool bDebugOverlapSphere = false;
	
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes { UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) };
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	TSubclassOf<AActor> TargetClass;
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	FGameplayTag TargetTag;
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	FGameplayTag TargetTagId;
	
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	float OverlapSphereRadius = 5000.f;
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	EFmBttFindTargetSelectionStrategy SelectionStrategy = EFmBttFindTargetSelectionStrategy::ClosestDistance;
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	bool bAllowNull = true;
	UPROPERTY(EditInstanceOnly, Category="FM Params")
	bool bFailIfNotFound = true;
};
