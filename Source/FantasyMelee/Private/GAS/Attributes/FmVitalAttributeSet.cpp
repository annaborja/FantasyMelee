// Copyright AB. All Rights Reserved.

#include "GAS/Attributes/FmVitalAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Tags/TempTags.h"

UFmVitalAttributeSet::UFmVitalAttributeSet()
{
	TagAttributeMap.Add(TAG_Attribute_Vital_Health, GetHealthAttribute);
	TagAttributeMap.Add(TAG_Attribute_Vital_MaxHealth, GetMaxHealthAttribute);
}

void UFmVitalAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
}
