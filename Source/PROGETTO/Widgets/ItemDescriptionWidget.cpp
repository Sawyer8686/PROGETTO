#include "ItemDescriptionWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "PROGETTO/Widgets/ChestWidget.h"
#include "PROGETTO/Widgets/InventoryWidget.h"
#include "Components/Button.h"

void UItemDescriptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UItemDescriptionWidget::OnCloseButtonClicked);
	}

	if (BackgroundBorder)
	{
		FLinearColor SfondoColor = FLinearColor(0.1f, 0.1f, 0.3f, 0.8f);
		BackgroundBorder->SetBrushColor(SfondoColor);	
	}
}

void UItemDescriptionWidget::SetDescriptionText(const FText& InDescription)
{
	if (DescriptionText)
	{
		DescriptionText->SetText(InDescription);
	}
}

void UItemDescriptionWidget::SetParentInventoryWidget(UInventoryWidget* Parent)
{
	ParentInventoryWidget = Parent;
}

void UItemDescriptionWidget::OnCloseButtonClicked()
{
	
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->SetIsEnabled(true);
		ParentInventoryWidget->UnregisterOpenDescription(this);
	}

	RemoveFromParent();

	// Re-enable the chest widget UI
	if (ParentChestWidget)
	{
		ParentChestWidget->SetIsEnabled(true);

		// Restore input mode and focus to the chest widget
		if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
		{
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;

			FInputModeUIOnly UIInput;
			// Ensure the chest root panel receives focus
			UIInput.SetWidgetToFocus(ParentChestWidget->GetRootWidget()->TakeWidget());
			UIInput.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(UIInput);
		}
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly UIInput;
		UIInput.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		if (ParentInventoryWidget)
		{
			UIInput.SetWidgetToFocus(ParentInventoryWidget->TakeWidget());
		}
		PC->SetInputMode(UIInput);
	}
	
}



