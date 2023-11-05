// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FmPlayerBlockingVolume.generated.h"

class UBoxComponent;

UCLASS()
class FANTASYMELEE_API AFmPlayerBlockingVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AFmPlayerBlockingVolume();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;
};
