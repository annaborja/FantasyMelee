// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FmPlayerInteractionComponent.generated.h"

class AFmPlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYMELEE_API UFmPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFmPlayerInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE TObjectPtr<const AActor> GetTargetInteractable() const { return TargetInteractable; }

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<AFmPlayerCharacter> Owner;
	
	UPROPERTY(VisibleAnywhere, Category="Actors")
	TObjectPtr<const AActor> TargetInteractable;
	
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceLength = 150.f;
};
