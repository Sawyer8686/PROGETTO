#include "ItemDescriptionWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
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
	// Deregistro questa descrizione dal widget Inventory
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->ClearOpenDescriptions();
	}

	// Rimuovo il widget dal viewport
	RemoveFromParent();
}



