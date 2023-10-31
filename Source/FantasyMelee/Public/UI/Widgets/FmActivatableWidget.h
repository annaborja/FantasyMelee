// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Interfaces/FmHudSettable.h"
#include "FmActivatableWidget.generated.h"

struct FGameplayTag;

UCLASS()
class FANTASYMELEE_API UFmActivatableWidget : public UCommonActivatableWidget, public IFmHudSettable
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	
	virtual void SetCustomHud(AFmHud* InCustomHud) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AFmHud> CustomHud;
	
	UFUNCTION(BlueprintImplementableEvent)
	TArray<UCommonUserWidget*> GetHudSettableChildren() const;

	UFUNCTION(BlueprintNativeEvent)
	void OnCustomHudSet();
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttributeValueBroadcast(const FGameplayTag& Tag, const float Value);

private:
	UPROPERTY(EditDefaultsOnly, Category="FM Params")
	ECommonInputMode InputMode = ECommonInputMode::Menu;
};
