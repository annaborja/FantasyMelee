// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FmInteractable.generated.h"

class AFmPlayerCharacter;
class AFmHud;
class AFmPlayerController;

UINTERFACE(MinimalAPI)
class UFmInteractable : public UInterface
{
	GENERATED_BODY()
};

class FANTASYMELEE_API IFmInteractable
{
	GENERATED_BODY()
	
public:
	virtual bool CanInteract(AFmPlayerCharacter* Player) const = 0;
	virtual void Interact(AFmPlayerController* PlayerController, AFmHud* Hud) = 0;
};
