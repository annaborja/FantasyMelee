// Copyright AB. All Rights Reserved.

#include "Scripting/FmFogVolume.h"

AFmFogVolume::AFmFogVolume(): AActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetWorldScale3D(FVector(5.f, 5.f, 5.f));
	StaticMeshComponent->SetMobility(EComponentMobility::Static);
	
	StaticMeshComponent->SetCanEverAffectNavigation(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AFmFogVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DynamicFogMaterial = StaticMeshComponent->CreateDynamicMaterialInstance(0, FogMaterial);
	DynamicFogMaterial->SetScalarParameterValue(FName(TEXT("Opacity")), FogOpacity);
}
