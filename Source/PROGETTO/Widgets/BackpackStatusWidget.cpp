#include "BackpackStatusWidget.h"
#include "Components/TextBlock.h"

void UBackpackStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackpackStatusText)
	{
		BackpackStatusText->SetText(FText::FromString("HAI CON TE UNO ZAINO"));
		BackpackStatusText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBackpackStatusWidget::SetBackpackStatusVisible(bool bVisible)
{
    if (!BackpackStatusText)
        return;

    if (bVisible)
    {
        // Quando è visibile, mostro entrambe le righe
        BackpackStatusText->SetText(
            FText::FromString(TEXT("HAI CON TE UNO ZAINO\nPress F to Drop"))
        );
        BackpackStatusText->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        BackpackStatusText->SetVisibility(ESlateVisibility::Hidden);
    }
}
