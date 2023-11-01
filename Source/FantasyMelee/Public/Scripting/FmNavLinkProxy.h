// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "FmNavLinkProxy.generated.h"

UENUM()
enum EFmNavLinkProxyNavigationType : uint8
{
	Jump,
	Mantle
};

UCLASS()
class FANTASYMELEE_API AFmNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category="FM Params")
	TEnumAsByte<EFmNavLinkProxyNavigationType> NavigationType = Jump;
	
	UFUNCTION()
	void HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);
};
