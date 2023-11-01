// Copyright AB. All Rights Reserved.

#include "Scripting/FmNavLinkProxy.h"

#include "Utils/Macros.h"

void AFmNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &ThisClass::HandleSmartLinkReached);
}

void AFmNavLinkProxy::HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
{
	switch (NavigationType)
	{
	case Jump:
		SCREEN_LOG("Jump", 4.f)
		break;
	case Mantle:
		SCREEN_LOG("Mantle", 4.f)
		break;
	default:
		break;
	}
}
