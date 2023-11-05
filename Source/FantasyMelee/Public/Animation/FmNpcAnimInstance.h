// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/FmAnimInstance.h"
#include "FmNpcAnimInstance.generated.h"

UCLASS()
class FANTASYMELEE_API UFmNpcAnimInstance : public UFmAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	TEnumAsByte<EFmNpcEmotionalState::Type> EmotionalState = EFmNpcEmotionalState::Neutral;
};
