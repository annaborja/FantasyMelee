// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/FmAnimInstance.h"
#include "FmPlayerAnimInstance.generated.h"

UCLASS()
class FANTASYMELEE_API UFmPlayerAnimInstance : public UFmAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bIsSliding = false;
};
