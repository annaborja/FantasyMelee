// Copyright AB. All Rights Reserved.

#include "Scripting/FmPlayerBlockingVolume.h"

#include "Components/BoxComponent.h"
#include "Utils/Constants.h"

AFmPlayerBlockingVolume::AFmPlayerBlockingVolume(): AActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetBoxExtent(FVector(68.f, 136.f, 176.f));
	BoxComponent->SetMobility(EComponentMobility::Static);
	
	BoxComponent->SetCanEverAffectNavigation(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetGenerateOverlapEvents(false);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(OBJECT_CHANNEL_PLAYER, ECR_Block);
}
