#include "ItemDescriptionWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UItemDescriptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Collego il callback al pulsante “CloseButton” (deve esistere nel Blueprint)
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UItemDescriptionWidget::OnCloseButtonClicked);
	}

	// Se abbiamo un Border bindato, possiamo impostarne il colore di sfondo
	if (BackgroundBorder)
	{
		// Esempio: rosso semi?trasparente (RGBA = 0.1, 0.1, 0.3, 0.8)
		FLinearColor SfondoColor = FLinearColor(0.1f, 0.1f, 0.3f, 0.8f);
		BackgroundBorder->SetBrushColor(SfondoColor);
		// Se vuoi un'immagine di sfondo anziché un semplice colore, puoi usare:
		// UTexture2D* MyTexture = ... ;
		// FSlateBrush Brush;
		// Brush.SetResourceObject(MyTexture);
		// BackgroundBorder->SetBrush(Brush);
	}
}

void UItemDescriptionWidget::SetDescriptionText(const FText& InDescription)
{
	if (DescriptionText)
	{
		DescriptionText->SetText(InDescription);
	}
}

void UItemDescriptionWidget::OnCloseButtonClicked()
{
	// Rimuovo il widget dal viewport
	RemoveFromParent();
}



