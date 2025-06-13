#include "ChestWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "ItemEntryWidget.h"           // usa entry widget generico
#include "PROGETTO/Components/ContainerComponent.h"
#include "PROGETTO/Widgets/ItemEntryWidget.h"
#include "ItemDescriptionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
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

void UChestWidget::OnEntryDescriptionRequested(ABaseItem* Item)
{
    UE_LOG(LogTemp, Warning, TEXT("UChestWidget: Description requested for %s"), *Item->ItemName);
    if (!DescriptionWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("UChestWidget: DescriptionWidgetClass is null!"));
        return;
    }
    APlayerController* PC = PlayerChar ? Cast<APlayerController>(PlayerChar->GetController()) : UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("UChestWidget: No valid PlayerController found."));
        return;
    }
    UItemDescriptionWidget* Desc = CreateWidget<UItemDescriptionWidget>(PC, DescriptionWidgetClass);
    if (!Desc)
    {
        UE_LOG(LogTemp, Error, TEXT("UChestWidget: Failed to create description widget."));
        return;
    }

    // Salva il riferimento per poterlo chiudere dopo
    //OpenDescriptions.Add(Desc);
    Desc->ParentChestWidget = this;

    // Disabilita il chest finché la descrizione è aperta
    //SetIsEnabled(false);
    
    Desc->SetDescriptionText(Item->Description);
    Desc->AddToViewport(1001);
    UE_LOG(LogTemp, Warning, TEXT("UChestWidget: DescriptionWidget added to viewport for %s."), *Item->ItemName);
}

void UChestWidget::RefreshLists()
{

    
    UE_LOG(LogTemp, Warning, TEXT("RefreshLists() invoked. Container has %d items"), ContainerComp ? ContainerComp->Inventory.Num() : -1);
    if (!ContainerComp || !PlayerChar || !ChestListBox || !PlayerListBox || !ItemEntryWidgetClass)
        return;

    ChestListBox->ClearChildren();
    PlayerListBox->ClearChildren();

    // CHest contents
    for (ABaseItem* It : ContainerComp->Inventory)
    {
        if (!It) continue;
        UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(GetWorld(), ItemEntryWidgetClass);
        if (!Entry) continue;

        Entry->SetupFromItem(It, PlayerChar);
        Entry->DescriptionButton->SetVisibility(ESlateVisibility::Visible);
        Entry->TakeButton->SetVisibility(ESlateVisibility::Visible);
        Entry->StoreButton->SetVisibility(ESlateVisibility::Collapsed);
        Entry->EquipButton->SetVisibility(ESlateVisibility::Collapsed);
        Entry->DiscardButton->SetVisibility(ESlateVisibility::Collapsed);

        //Entry->OnTakeAction.AddDynamic(this, &UChestWidget::OnTakeFromChest);
        Entry->OnTakeAction.BindUObject(this, &UChestWidget::OnTakeFromChest);
        Entry->OnDescriptionRequested.AddDynamic(this, &UChestWidget::OnEntryDescriptionRequested);

        ChestListBox->AddChild(Entry);
    }

    // Player inventory for storing
    if (UInventoryComponent* InvComp = PlayerChar->FindComponentByClass<UInventoryComponent>())
    {
        for (ABaseItem* It : InvComp->Inventory)
        {
            if (!It) continue;
            UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(GetWorld(), ItemEntryWidgetClass);
            if (!Entry) continue;

            Entry->SetupFromItem(It, PlayerChar);
            Entry->DescriptionButton->SetVisibility(ESlateVisibility::Visible);
            Entry->TakeButton->SetVisibility(ESlateVisibility::Collapsed);
            Entry->StoreButton->SetVisibility(ESlateVisibility::Visible);
            Entry->EquipButton->SetVisibility(ESlateVisibility::Collapsed);
            Entry->DiscardButton->SetVisibility(ESlateVisibility::Collapsed);

            Entry->OnStoreAction.BindUObject(this, &UChestWidget::OnStoreToChest);
            Entry->OnDescriptionRequested.AddDynamic(this, &UChestWidget::OnEntryDescriptionRequested);

            PlayerListBox->AddChild(Entry);
        }
    }
}

void UChestWidget::OnCloseClicked()
{
    if (ContainerComp && PlayerChar)
        ContainerComp->ToggleContainer(PlayerChar);

    // Chiudi tutti i description aperti
    for (UItemDescriptionWidget* Desc : OpenDescriptions)
    {
        if (Desc && Desc->IsInViewport())
            Desc->RemoveFromParent();
    }
    OpenDescriptions.Empty();

    // Chiudi il chest stesso
    RemoveFromParent();

    // Ripristina input mode di gioco
    if (PlayerChar)
    {
        if (APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController()))
        {
            PC->bShowMouseCursor = false;
            FInputModeGameOnly GameInput;
            PC->SetInputMode(GameInput);
        }
    }
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

