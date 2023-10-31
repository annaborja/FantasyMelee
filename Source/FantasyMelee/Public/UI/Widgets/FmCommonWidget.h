// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Interfaces/FmHudSettable.h"
#include "FmCommonWidget.generated.h"

struct FGameplayTag;

UCLASS()
class FANTASYMELEE_API UFmCommonWidget : public UCommonUserWidget, public IFmHudSettable
{
	GENERATED_BODY()

public:
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
};
