// Copyright AB. All Rights Reserved.

#include "Scripting/FmBoxTrigger.h"

#include "AI/FmAiController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/NPCs/FmMajorNpcCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/Constants.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

AFmBoxTrigger::AFmBoxTrigger(): AActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetBoxExtent(BoxExtent);
	BoxComponent->SetCollisionResponseToChannel(OBJECT_CHANNEL_PLAYER, ECR_Overlap);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxBeginOverlap);
}

void AFmBoxTrigger::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);

	if (TagId.IsValid())
	{
		TagContainer.AddTag(TagId);
	}
}

void AFmBoxTrigger::OnBoxBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (auto& TriggerEffect : TriggerEffects)
	{
		if (TriggerEffect.bExecuteOnceOnly && TriggerEffect.bExecuted) continue;
		
		switch (TriggerEffect.Type)
		{
		case DestroyActor:
			TriggerEffect.bExecuted = HandleDestroyActor(TriggerEffect, OtherActor);
			break;
		case GrantTag:
			TriggerEffect.bExecuted = HandleGrantTag(TriggerEffect, OtherActor);
			break;
		case ProcessEntityTagSpec:
			TriggerEffect.bExecuted = HandleProcessEntityTagSpec(TriggerEffect, OtherActor);
			break;
		case SetMoveTarget:
			TriggerEffect.bExecuted = HandleSetMoveTarget(TriggerEffect, OtherActor);
			break;
		default:
			break;
		}
	}
}

bool AFmBoxTrigger::PassesClassFilter(const FFmTriggerEffect& TriggerEffect, const AActor* OtherActor)
{
	return TriggerEffect.ActorClassFilter.ContainsByPredicate([&OtherActor](const TSubclassOf<AActor> ActorSubclass)
		{
			return OtherActor->IsA(ActorSubclass);
		});
}

bool AFmBoxTrigger::HandleDestroyActor(const FFmTriggerEffect& TriggerEffect, AActor* OtherActor)
{
	if (!PassesClassFilter(TriggerEffect, OtherActor)) return false;

	OtherActor->Destroy();
	
	return true;
}

bool AFmBoxTrigger::HandleGrantTag(const FFmTriggerEffect& TriggerEffect, AActor* OtherActor)
{
	if (!PassesClassFilter(TriggerEffect, OtherActor)) return false;

	if (const auto TagSpecable = Cast<IFmTagSpecGrantable>(OtherActor))
	{
		for (const auto& TagSpec : TriggerEffect.TagSpecs)
		{
			TagSpecable->GrantTagSpec(TagSpec);
		}
	}

	return true;
}

bool AFmBoxTrigger::HandleProcessEntityTagSpec(const FFmTriggerEffect& TriggerEffect, const AActor* OtherActor) const
{
	if (!PassesClassFilter(TriggerEffect, OtherActor)) return false;

	UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(this, TriggerEffect.EntityTagSpec);
	
	return true;
}

bool AFmBoxTrigger::HandleSetMoveTarget(const FFmTriggerEffect& TriggerEffect, const AActor* OtherActor) const
{
	if (!PassesClassFilter(TriggerEffect, OtherActor)) return false;
	
	const auto& TargetTagIdA = TriggerEffect.TargetA.TagId;
	const auto& TargetTagIdB = TriggerEffect.TargetB.TagId;
	AActor* AffectedActor = nullptr;
	AActor* NewMoveTarget = nullptr;
	
	TArray<AActor*> OverlapActorsA;
	UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), TriggerEffect.TargetA.OverlapSphereRadius,
		TriggerEffect.TargetA.ObjectTypes, TriggerEffect.TargetA.TargetClass, TArray<AActor*>(), OverlapActorsA);
		
	for (const auto ActorA : OverlapActorsA)
	{
		if (!TargetTagIdA.IsValid())
		{
			AffectedActor = ActorA;
			break;
		}
		
		if (const auto TagIdableA = Cast<IFmTagIdable>(ActorA))
		{
			if (const auto& TagIdA = TagIdableA->GetTagId(); TagIdA.IsValid() && TagIdA.MatchesTagExact(TargetTagIdA))
			{
				AffectedActor = ActorA;
				break;
			}
		}
	}

	// If no target class is set for target B, set the move target to null.
	if (TriggerEffect.TargetB.TargetClass)
	{
		TArray<AActor*> OverlapActorsB;
		UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), TriggerEffect.TargetB.OverlapSphereRadius,
			TriggerEffect.TargetB.ObjectTypes, TriggerEffect.TargetB.TargetClass, TArray<AActor*>(), OverlapActorsB);
	
		for (const auto ActorB : OverlapActorsB)
		{
			if (!TargetTagIdB.IsValid())
			{
				NewMoveTarget = ActorB;
				break;
			}
		
			if (const auto TagIdableB = Cast<IFmTagIdable>(ActorB))
			{
				if (const auto& TagIdB = TagIdableB->GetTagId(); TagIdB.IsValid() && TagIdB.MatchesTagExact(TargetTagIdB))
				{
					NewMoveTarget = ActorB;
					break;
				}
			}
		}
	}

	if (const auto Npc = Cast<AFmMajorNpcCharacter>(AffectedActor))
	{
		if (const auto AiController = Cast<AFmAiController>(Npc->GetController()))
		{
			if (const auto BlackboardComponent = AiController->GetBlackboardComponent())
			{
				BlackboardComponent->SetValueAsObject(FName(BLACKBOARD_KEY_MOVE_TARGET), NewMoveTarget);
			}
		}
	}

	return true;
}
