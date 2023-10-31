// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/FmActivatableWidget.h"
#include "FmDialogueOverlayWidget.generated.h"

class AFmMajorNpcCharacter;
struct FFmDialogueOptionData;
struct FFmDialogueStepData;

UCLASS()
class FANTASYMELEE_API UFmDialogueOverlayWidget : public UFmActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayDialogueOptions(const AFmMajorNpcCharacter* Npc, const TArray<FFmDialogueOptionData>& DialogueOptions);
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayDialogueStep(const AFmMajorNpcCharacter* Npc, const FFmDialogueStepData& DialogueStep);
};
