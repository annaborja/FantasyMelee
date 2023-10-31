// Copyright AB. All Rights Reserved.

#include "Characters/Player/FmPlayerState.h"

#include "GAS/FmAbilitySystemComponent.h"
#include "GAS/Attributes/FmPrimaryAttributeSet.h"
#include "GAS/Attributes/FmVitalAttributeSet.h"

AFmPlayerState::AFmPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UFmAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	PrimaryAttributeSet = CreateDefaultSubobject<UFmPrimaryAttributeSet>(TEXT("PrimaryAttributeSet"));
	VitalAttributeSet = CreateDefaultSubobject<UFmVitalAttributeSet>(TEXT("VitalAttributeSet"));
}

TArray<UFmBaseAttributeSet*> AFmPlayerState::GetAttributeSets() const
{
	return TArray<UFmBaseAttributeSet*> {
		GetPrimaryAttributeSet(),
		GetVitalAttributeSet()
	};
}
