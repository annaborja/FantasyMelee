// Copyright AB. All Rights Reserved.

#include "AI/Tasks/FmBtTask_FindTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

EBTNodeResult::Type UFmBtTask_FindTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const auto AiController = OwnerComp.GetAIOwner())
	{
		if (const auto AiPawn = AiController->GetPawn())
		{
			if (const auto BlackboardComponent = OwnerComp.GetBlackboardComponent())
			{
				const auto SpherePosition = AiPawn->GetActorLocation();

				TArray<AActor*> OverlapActors;
				UKismetSystemLibrary::SphereOverlapActors(AiPawn, SpherePosition, OverlapSphereRadius,
					ObjectTypes, TargetClass, TArray<AActor*> { AiPawn }, OverlapActors);

				if (bDebugOverlapSphere)
				{
					UKismetSystemLibrary::DrawDebugSphere(AiPawn, SpherePosition, OverlapSphereRadius, 12, FColor::Orange, 5.f);
				}
				
				TArray<AActor*> CandidateActors;

				for (const auto Actor : OverlapActors)
				{
					if (TargetTagId.IsValid() && UFmBlueprintFunctionLibrary::HasTagId(Actor, TargetTagId))
					{
						CandidateActors.Add(Actor);
						break;
					}

					if ((TargetTag.IsValid() && UFmBlueprintFunctionLibrary::HasTag(Actor, TargetTag)) ||
						(!TargetTagId.IsValid() && !TargetTag.IsValid()))
					{
						CandidateActors.Add(Actor);
					}
				}
		
				AActor* NewTarget = nullptr;

				if (CandidateActors.Num() > 0)
				{
					int32 NewTargetIndex = 0;
			
					switch (SelectionStrategy)
					{
					case EFmBttFindTargetSelectionStrategy::Random:
						NewTargetIndex = FMath::RandRange(0, CandidateActors.Num() - 1);
					case EFmBttFindTargetSelectionStrategy::ClosestDistance:
					default:
						CandidateActors.Sort([AiPawn](const AActor& ActorA, const AActor& ActorB)
						{
							return AiPawn->GetDistanceTo(&ActorA) < AiPawn->GetDistanceTo(&ActorB);
						});
					}

					if (CandidateActors.IsValidIndex(NewTargetIndex))
					{
						NewTarget = CandidateActors[NewTargetIndex];
					}
				}

				if (NewTarget || bAllowNull)
				{
					BlackboardComponent->SetValueAsObject(TargetSelector.SelectedKeyName, NewTarget);
				}
				
				if (!NewTarget && bFailIfNotFound) return EBTNodeResult::Failed;
		
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}
