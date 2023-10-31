// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/FmCommonWidget.h"
#include "FmPersistentOverlayWidget.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
class UFmActivatableWidget;
class UFmDialogueOverlayWidget;

UCLASS()
class FANTASYMELEE_API UFmPersistentOverlayWidget : public UFmCommonWidget
{
	GENERATED_BODY()
	
public:
	UFmDialogueOverlayWidget* OpenDialogueFlow() const;
	void OpenMainMenu() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	UCommonActivatableWidgetContainerBase* GetDialogueWidgetContainer() const;
	UFUNCTION(BlueprintImplementableEvent)
	UCommonActivatableWidgetContainerBase* GetMenuWidgetContainer() const;
	
	virtual void NativePreConstruct() override;

private:
	UFUNCTION()
	void OnDisplayedWidgetChanged(UCommonActivatableWidget* Widget) const;
};
