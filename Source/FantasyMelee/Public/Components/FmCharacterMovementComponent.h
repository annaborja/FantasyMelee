// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FmCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
namespace EFmCustomMovementMode
{
	enum Type : uint8
	{
		None
	};
}

UCLASS()
class FANTASYMELEE_API UFmCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;
	
	void ToggleSprint(const bool bInWantsToSprint);

protected:
	UPROPERTY(EditAnywhere, Category="FM Params|Sprint")
	float MaxSprintSpeed = 900.f;
	
	bool bWantsToSprint = false;
};
