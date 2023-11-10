// Copyright AB. All Rights Reserved.

#include "Scripting/FmNavLinkProxy.h"

#include "Characters/NPCs/FmNpcCharacter.h"
#include "Utils/Macros.h"

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
			SCREEN_LOG("Mantle", 4.f)
			break;
		default:
			break;
		}
	}
}
