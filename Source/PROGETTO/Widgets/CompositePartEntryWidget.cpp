#include "CompositePartEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCompositePartEntryWidget::Setup(const FString& Name, UTexture2D* IconTexture)
{
    if (PartName)
        PartName->SetText(FText::FromString(Name));

    if (PartIcon && IconTexture)
        PartIcon->SetBrushFromTexture(IconTexture);
}

