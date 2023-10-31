// Copyright AB. All Rights Reserved.

#include "UI/Widgets/FmCommonWidget.h"

#include "UI/FmHud.h"

void UFmCommonWidget::SetCustomHud(AFmHud* InCustomHud)
{
	if (CustomHud == InCustomHud) return;
	
	CustomHud = InCustomHud;
	OnCustomHudSet();
}

void UFmCommonWidget::OnCustomHudSet_Implementation()
{
	if (!CustomHud) return;
	
	CustomHud->AttributeValueDelegate.AddDynamic(this, &ThisClass::OnAttributeValueBroadcast);

	for (const auto Widget : GetHudSettableChildren())
	{
		if (const auto HudSettable = Cast<IFmHudSettable>(Widget))
		{
			HudSettable->SetCustomHud(CustomHud);
		}
	}
}
