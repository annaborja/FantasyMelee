// Copyright AB. All Rights Reserved.

#include "UI/Widgets/FmPersistentOverlayWidget.h"

#include "Input/CommonUIActionRouterBase.h"
#include "UI/FmHud.h"
#include "UI/Widgets/FmDialogueOverlayWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UFmDialogueOverlayWidget* UFmPersistentOverlayWidget::OpenDialogueFlow() const
{
	UFmDialogueOverlayWidget* Widget = nullptr;

	if (const auto DialogueWidgetContainer = GetDialogueWidgetContainer(); DialogueWidgetContainer && CustomHud)
	{
		if (const auto WidgetClass = CustomHud->GetDialogueOverlayClass())
		{
			// `AddWidget()` will handle grabbing an existing instance of the given widget class (if any).
			DialogueWidgetContainer->AddWidget<UFmDialogueOverlayWidget>(WidgetClass, [this, &Widget](UFmDialogueOverlayWidget& AddedWidget)
			{
				Widget = &AddedWidget;
				AddedWidget.SetCustomHud(CustomHud);
			});
		}
	}
	
	return Widget;
}

void UFmPersistentOverlayWidget::OpenMainMenu() const
{
	if (const auto MenuWidgetContainer = GetMenuWidgetContainer(); MenuWidgetContainer && CustomHud)
	{
		if (const auto WidgetClass = CustomHud->GetMenuSwitcherClass())
		{
			// `AddWidget()` will handle grabbing an existing instance of the given widget class (if any).
			MenuWidgetContainer->AddWidget<UFmActivatableWidget>(WidgetClass, [this](UFmActivatableWidget& AddedWidget)
			{
				AddedWidget.SetCustomHud(CustomHud);
			});
		}
	}
}

void UFmPersistentOverlayWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (const auto DialogueWidgetContainer = GetDialogueWidgetContainer())
	{
		DialogueWidgetContainer->OnDisplayedWidgetChanged().AddUObject(this, &ThisClass::OnDisplayedWidgetChanged);
	}

	if (const auto MenuWidgetContainer = GetMenuWidgetContainer())
	{
		MenuWidgetContainer->OnDisplayedWidgetChanged().AddUObject(this, &ThisClass::OnDisplayedWidgetChanged);
	}
}

void UFmPersistentOverlayWidget::OnDisplayedWidgetChanged(UCommonActivatableWidget* Widget) const
{
	// If the final activatable widget has been deactivated, reset the UI input config
	// (otherwise, Common UI will use the input config of the most recent active widget).
	if (!Widget)
	{
		if (const auto ActionRouter = UCommonUIActionRouterBase::Get(*this))
		{
			ActionRouter->SetActiveUIInputConfig(
				FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently));
		}
	}
}
