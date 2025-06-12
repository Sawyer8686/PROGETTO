#include "CompositePartsWidget.h"
#include "CompositePartEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Components/InventoryComponent.h"

void UCompositePartsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ConfirmButton)
        ConfirmButton->OnClicked.AddDynamic(this, &UCompositePartsWidget::OnConfirmClicked);
    if (CancelButton)
        CancelButton->OnClicked.AddDynamic(this, &UCompositePartsWidget::OnCancelClicked);
}

void UCompositePartsWidget::SetupForComposite(ACompositeItem* InComposite, APROGETTOCharacter* InChar)
{
    PendingComposite = InComposite;
    OwningCharacter = InChar;

    if (PartsListBox)
        PartsListBox->ClearChildren();

    if (!PendingComposite || !CompositePartEntryWidgetClass)
        return;

    for (auto& PartClass : PendingComposite->PartClasses)
    {
        if (!PartClass) continue;
        ABaseItem* DefaultPart = PartClass->GetDefaultObject<ABaseItem>();
        if (!DefaultPart) continue;

        UCompositePartEntryWidget* Entry = CreateWidget<UCompositePartEntryWidget>(
            this, CompositePartEntryWidgetClass);
        if (Entry)
        {
            Entry->Setup(DefaultPart->ItemName, DefaultPart->Icon);
            PartsListBox->AddChild(Entry);
        }
    }

    // **Qui disabiliti il Confirm se manca spazio**
    bool bCanDismantle = false;
    if (OwningCharacter)
    {
        if (UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>())
        {
            bCanDismantle = InvComp->HasSpaceForParts(PendingComposite->PartClasses);
        }
    }
    if (ConfirmButton)
    {
        ConfirmButton->SetIsEnabled(bCanDismantle);
    }
}

void UCompositePartsWidget::OnConfirmClicked()
{
    if (!OwningCharacter || !PendingComposite)
        return;

    if (UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>())
    {
        InvComp->DismantleItem(PendingComposite);
    }
    RemoveFromParent();
}

void UCompositePartsWidget::OnCancelClicked()
{
    RemoveFromParent();
}

