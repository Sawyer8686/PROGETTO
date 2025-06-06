#include "EquipSlotSelectionWidget.h"
#include "Components/Button.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Actors/BaseItem.h"


void UEquipSlotSelectionWidget::SetupForItem(ABaseItem* Item, APROGETTOCharacter* Character)
{
    PendingItem = Item;
    OwningCharacter = Character;

    // 1) Disabilito tutti i bottoni di default
    //if (HeadButton)      HeadButton->SetIsEnabled(false);
    //if (RightHandButton) RightHandButton->SetIsEnabled(false);
    //if (LeftHandButton)  LeftHandButton->SetIsEnabled(false);
    //if (TorsoButton)     TorsoButton->SetIsEnabled(false);
    //if (RightLegButton)  RightLegButton->SetIsEnabled(false);
    //if (LeftLegButton)   LeftLegButton->SetIsEnabled(false);

    PendingItem->SetButtonDisabledBlack(HeadButton);
    PendingItem->SetButtonDisabledBlack(RightHandButton);
    PendingItem->SetButtonDisabledBlack(LeftHandButton);
    PendingItem->SetButtonDisabledBlack(TorsoButton);
    PendingItem->SetButtonDisabledBlack(RightLegButton);
    PendingItem->SetButtonDisabledBlack(LeftLegButton);

    // 2) Se non ho un item o un character valido, resto così
    if (!PendingItem || !OwningCharacter || PendingItem->ItemType != EItemType::Equippable)
        return;

    // 3) Abilito solo i bottoni corrispondenti agli slot consentiti
    const TArray<EEquipmentSlot>& Allowed = PendingItem->AllowedEquipmentSlots;
    for (EEquipmentSlot ASlot : Allowed)
    {
        switch (ASlot)
        {
        case EEquipmentSlot::Head:
            if (HeadButton) HeadButton->SetIsEnabled(true);
            break;

        case EEquipmentSlot::RightHand:
            if (RightHandButton) RightHandButton->SetIsEnabled(true);
            break;

        case EEquipmentSlot::LeftHand:
            if (LeftHandButton) LeftHandButton->SetIsEnabled(true);
            break;

        case EEquipmentSlot::Torso:
            if (TorsoButton) TorsoButton->SetIsEnabled(true);
            break;

        case EEquipmentSlot::RightLeg:
            if (RightLegButton) RightLegButton->SetIsEnabled(true);
            break;

        case EEquipmentSlot::LeftLeg:
            if (LeftLegButton) LeftLegButton->SetIsEnabled(true);
            break;

        default:
            break;
        }
    }
}

void UEquipSlotSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (HeadButton)        HeadButton->OnClicked.AddDynamic(this, &UEquipSlotSelectionWidget::OnHeadClicked);
    if (RightHandButton)   RightHandButton->OnClicked.AddDynamic(this, &UEquipSlotSelectionWidget::OnRightHandClicked);
    if (LeftHandButton)    LeftHandButton->OnClicked.AddDynamic(this, &UEquipSlotSelectionWidget::OnLeftHandClicked);
    if (TorsoButton)       TorsoButton->OnClicked.AddDynamic(this, &UEquipSlotSelectionWidget::OnTorsoClicked);
    if (RightLegButton)    RightLegButton->OnClicked.AddDynamic(this, &UEquipSlotSelectionWidget::OnRightLegClicked);
    if (LeftLegButton)     LeftLegButton->OnClicked.AddDynamic(this, &UEquipSlotSelectionWidget::OnLeftLegClicked);
}

void UEquipSlotSelectionWidget::OnHeadClicked()
{
    if (OwningCharacter && PendingItem)
    {
        OwningCharacter->EquipItemToSlot(PendingItem, EEquipmentSlot::Head);
    }
    if (OwningCharacter->InventoryWidgetInstance)
    {
        OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
            OwningCharacter->Inventory,
            OwningCharacter->CurrentCarryWeight,
            OwningCharacter->MaxCarryWeight
        );
    }

    RemoveFromParent();
}

void UEquipSlotSelectionWidget::OnRightHandClicked()
{
    if (OwningCharacter && PendingItem)
    {
        OwningCharacter->EquipItemToSlot(PendingItem, EEquipmentSlot::RightHand);
    }

    if (OwningCharacter->InventoryWidgetInstance)
    {
        OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
            OwningCharacter->Inventory,
            OwningCharacter->CurrentCarryWeight,
            OwningCharacter->MaxCarryWeight
        );
    }
    RemoveFromParent();
}

void UEquipSlotSelectionWidget::OnLeftHandClicked()
{
    if (OwningCharacter && PendingItem)
    {
        OwningCharacter->EquipItemToSlot(PendingItem, EEquipmentSlot::LeftHand);
    }

    if (OwningCharacter->InventoryWidgetInstance)
    {
        OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
            OwningCharacter->Inventory,
            OwningCharacter->CurrentCarryWeight,
            OwningCharacter->MaxCarryWeight
        );
    }
    RemoveFromParent();
}

void UEquipSlotSelectionWidget::OnTorsoClicked()
{
    if (OwningCharacter && PendingItem)
    {
        OwningCharacter->EquipItemToSlot(PendingItem, EEquipmentSlot::Torso);
    }

    if (OwningCharacter->InventoryWidgetInstance)
    {
        OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
            OwningCharacter->Inventory,
            OwningCharacter->CurrentCarryWeight,
            OwningCharacter->MaxCarryWeight
        );
    }
    RemoveFromParent();
}

void UEquipSlotSelectionWidget::OnRightLegClicked()
{
    if (OwningCharacter && PendingItem)
    {
        OwningCharacter->EquipItemToSlot(PendingItem, EEquipmentSlot::RightLeg);
    }

    if (OwningCharacter->InventoryWidgetInstance)
    {
        OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
            OwningCharacter->Inventory,
            OwningCharacter->CurrentCarryWeight,
            OwningCharacter->MaxCarryWeight
        );
    }
    RemoveFromParent();
}

void UEquipSlotSelectionWidget::OnLeftLegClicked()
{
    if (OwningCharacter && PendingItem)
    {
        OwningCharacter->EquipItemToSlot(PendingItem, EEquipmentSlot::LeftLeg);
    }

    if (OwningCharacter->InventoryWidgetInstance)
    {
        OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
            OwningCharacter->Inventory,
            OwningCharacter->CurrentCarryWeight,
            OwningCharacter->MaxCarryWeight
        );
    }
    RemoveFromParent();
}

