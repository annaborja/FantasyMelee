// Copyright AB. All Rights Reserved.

#include "GAS/Attributes/FmPrimaryAttributeSet.h"

#include "Tags/TempTags.h"

UFmPrimaryAttributeSet::UFmPrimaryAttributeSet()
{
	TagAttributeMap.Add(TAG_Attribute_Primary_Creativity, GetCreativityAttribute);
	TagAttributeMap.Add(TAG_Attribute_Primary_Dexterity, GetDexterityAttribute);
	TagAttributeMap.Add(TAG_Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagAttributeMap.Add(TAG_Attribute_Primary_Resilience, GetResilienceAttribute);
	TagAttributeMap.Add(TAG_Attribute_Primary_Speed, GetSpeedAttribute);
	TagAttributeMap.Add(TAG_Attribute_Primary_Strength, GetStrengthAttribute);
}
