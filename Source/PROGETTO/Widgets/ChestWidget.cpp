#include "ChestWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "ItemEntryWidget.h"           // usa entry widget generico
#include "PROGETTO/Components/ContainerComponent.h"
#include "PROGETTO/Widgets/ItemEntryWidget.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include <PROGETTO/Actors/CompositeItem.h>

void UChestWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (CloseButton)
        CloseButton->OnClicked.AddDynamic(this, &UChestWidget::OnCloseClicked);
}

void UChestWidget::SetupForContainer(UContainerComponent* InContainer, APROGETTOCharacter* InPlayer)
{
    ContainerComp = InContainer;
    PlayerChar = InPlayer;
    RefreshLists();
}

void UChestWidget::RefreshLists()
{

    
    UE_LOG(LogTemp, Warning, TEXT("RefreshLists() invoked. Container has %d items"), ContainerComp ? ContainerComp->Inventory.Num() : -1);

    if (!ContainerComp || !PlayerChar || !ChestListBox || !PlayerListBox)
        return;

    ChestListBox->ClearChildren();

    for (ABaseItem* It : ContainerComp->Inventory)
    {
        if (!ItemEntryWidgetClass)
        {
            UE_LOG(LogTemp, Error, TEXT("ItemEntryWidgetClass è NULL!"));
            return;
        }

        if (!It) continue;
        UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(this, ItemEntryWidgetClass);
        if (!Entry) continue;

        Entry->Item = It;
        Entry->OwningCharacter = PlayerChar;

        Entry->EquipButton->SetVisibility(ESlateVisibility::Collapsed);
        Entry->DiscardButton->SetVisibility(ESlateVisibility::Collapsed);
        
        Entry->TakeButton->SetVisibility(ESlateVisibility::Visible);
        Entry->StoreButton->SetVisibility(ESlateVisibility::Collapsed);
        Entry->OnTakeAction = FOnItemAction::CreateUObject(this, &UChestWidget::OnTakeFromChest);

        
        ChestListBox->AddChild(Entry);
    }

    
    PlayerListBox->ClearChildren();

    if (UInventoryComponent* InvComp = PlayerChar->FindComponentByClass<UInventoryComponent>())
    {
        for (ABaseItem* It : InvComp->Inventory)
        {
            if (!It) continue;
            UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(this, ItemEntryWidgetClass);
            if (!Entry) continue;

            Entry->Item = It;
            Entry->OwningCharacter = PlayerChar;

            Entry->TakeButton->SetVisibility(ESlateVisibility::Collapsed);
            Entry->StoreButton->SetVisibility(ESlateVisibility::Visible);

            UE_LOG(LogTemp, Warning,
                TEXT("PLayerEntry '%s' — Take: %d, Store: %d"),
                *It->ItemName,
                (int)Entry->TakeButton->GetVisibility(),
                (int)Entry->StoreButton->GetVisibility()
            );

            Entry->OnStoreAction = FOnItemAction::CreateUObject(this, &UChestWidget::OnStoreToChest);

            Entry->EquipButton->SetVisibility(ESlateVisibility::Collapsed);
            Entry->DiscardButton->SetVisibility(ESlateVisibility::Collapsed);
           

            PlayerListBox->AddChild(Entry);
        }
    }
}

void UChestWidget::OnCloseClicked()
{
    if (ContainerComp && PlayerChar)
        ContainerComp->ToggleContainer(PlayerChar);
}

void UChestWidget::OnTakeFromChest(ABaseItem* Item)
{
    if (!ContainerComp || !PlayerChar) return;

    if (ContainerComp->RemoveItemFromContainer(Item))
    {
        if (UInventoryComponent* Inv = PlayerChar->FindComponentByClass<UInventoryComponent>())
        {
            Inv->AddItemToInventory(Item);
        }
        RefreshLists();
    }
}

void UChestWidget::OnStoreToChest(ABaseItem* Item)
{
    if (!ContainerComp || !PlayerChar) return;

    if (UInventoryComponent* Inv = PlayerChar->FindComponentByClass<UInventoryComponent>())
    {
        if (Inv->RemoveItemFromInventory(Item))
        {
            ContainerComp->AddItemToContainer(Item);
            RefreshLists();
        }
    }
}

