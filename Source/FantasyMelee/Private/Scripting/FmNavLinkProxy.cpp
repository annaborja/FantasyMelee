// Copyright AB. All Rights Reserved.

#include "Scripting/FmNavLinkProxy.h"

#include "Characters/NPCs/FmNpcCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void AFmNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &ThisClass::HandleSmartLinkReached);
}

void AFmNavLinkProxy::HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
{
	if (const auto Npc = Cast<AFmNpcCharacter>(MovingActor))
	{
		switch (NavigationType)
		{
		case Jump:
			Npc->JumpToLocation(DestinationPoint, MovementDuration);
			break;
		case Mantle:
			{
				const auto LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DestinationPoint);
				Npc->SetActorRotation(FRotator(0.f, LookRotation.Yaw, LookRotation.Roll));
				Npc->Mantle();
				Npc->SetTargetMoveLocation(DestinationPoint);
				break;
			}
		default:
			break;
		}
	}
}
