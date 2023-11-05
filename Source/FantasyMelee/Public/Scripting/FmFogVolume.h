// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FmFogVolume.generated.h"

UCLASS(Abstract)
class FANTASYMELEE_API AFmFogVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AFmFogVolume();
	
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="FM Assets")
	TObjectPtr<UMaterialInterface> FogMaterial;

	UPROPERTY(EditAnywhere, Category="FM Params")
	float FogOpacity = 1.f;
	
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicFogMaterial;
};
