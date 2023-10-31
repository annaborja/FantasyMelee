// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FmAnimInstance.generated.h"

UCLASS()
class FANTASYMELEE_API UFmAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float MovementDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float VerticalSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	bool bIsCrouched = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	bool bIsFalling = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	bool bIsMoving = false;
};
