// Copyright AB. All Rights Reserved.

#include "UI/Widgets/FmActivatableWidget.h"

#include "UI/FmHud.h"

TOptional<FUIInputConfig> UFmActivatableWidget::GetDesiredInputConfig() const
{
	if (InputMode == ECommonInputMode::Menu)
	{
		// In Menu input mode, UE intentionally registers game inputs if mouse capture is enabled.
		return FUIInputConfig(InputMode, EMouseCaptureMode::NoCapture);
	}
	
	return FUIInputConfig(InputMode, EMouseCaptureMode::CapturePermanently);
}

void UFmActivatableWidget::SetCustomHud(AFmHud* InCustomHud)
{
	if (CustomHud == InCustomHud) return;
	
	CustomHud = InCustomHud;
	OnCustomHudSet();
}

void UFmActivatableWidget::OnCustomHudSet_Implementation()
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
