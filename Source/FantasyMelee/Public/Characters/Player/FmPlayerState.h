// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GAS/FmAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "FmPlayerState.generated.h"

class UFmBaseAttributeSet;
class UFmPrimaryAttributeSet;
class UFmVitalAttributeSet;

UCLASS()
class FANTASYMELEE_API AFmPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFmPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	TArray<UFmBaseAttributeSet*> GetAttributeSets() const;
	
	FORCEINLINE UFmPrimaryAttributeSet* GetPrimaryAttributeSet() const { return PrimaryAttributeSet; }
	FORCEINLINE UFmVitalAttributeSet* GetVitalAttributeSet() const { return VitalAttributeSet; }

private:
	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UFmAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, Category="GAS|Attributes")
	TObjectPtr<UFmPrimaryAttributeSet> PrimaryAttributeSet;
	UPROPERTY(VisibleAnywhere, Category="GAS|Attributes")
	TObjectPtr<UFmVitalAttributeSet> VitalAttributeSet;
};
