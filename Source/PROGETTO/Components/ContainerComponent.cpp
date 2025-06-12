#include "ContainerComponent.h"
#include "PROGETTO/Widgets/ChestWidget.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UContainerComponent::UContainerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UContainerComponent::HasSpaceForItem(ABaseItem* Item) const
{
    if (!Item) return false;
    if (Inventory.Num() >= InventorySize) return false;
    return true;
}

bool UContainerComponent::AddItemToContainer(ABaseItem* Item)
{
    if (!Item || !HasSpaceForItem(Item))
        return false;

    Inventory.Add(Item);
    
    return true;
}

bool UContainerComponent::RemoveItemFromContainer(ABaseItem* Item)
{
    if (!Item) return false;
    if (Inventory.Remove(Item) > 0)
    {
       
        return true;
    }
    return false;
}

void UContainerComponent::ToggleContainer(APROGETTOCharacter* ForCharacter)
{
    if (!ForCharacter || !ChestWidgetClass) return;
    CachedCharacter = ForCharacter;

    APlayerController* PC = Cast<APlayerController>(ForCharacter->GetController());
    if (!PC) return;

    // CREA UNA SOLA VOLTA IL WIDGET
    if (!ChestWidgetInstance)
    {
        ChestWidgetInstance = CreateWidget<UChestWidget>(PC, ChestWidgetClass);
        ChestWidgetInstance->SetupForContainer(this, ForCharacter);
        ChestWidgetInstance->AddToViewport(500);
    }

    if (ChestWidgetInstance->IsVisible())
    {
        // ——— CHIUSURA BAULE ———
        ChestWidgetInstance->SetVisibility(ESlateVisibility::Hidden);

        // Rimuovi cursore UI
        PC->SetShowMouseCursor(false);
        // Ripristina input di gioco
        PC->SetInputMode(FInputModeGameOnly());
        ForCharacter->EnableInput(PC);
    }
    else
    {
        // ——— APERTURA BAULE ———
        ChestWidgetInstance->SetupForContainer(this, ForCharacter);
        ChestWidgetInstance->SetVisibility(ESlateVisibility::Visible);

        // Mostra solo il cursore per UI
        PC->SetShowMouseCursor(true);
        // Modalità solo UI: blocca ogni input di gioco
        FInputModeUIOnly UIMode;
        UIMode.SetWidgetToFocus(ChestWidgetInstance->TakeWidget());
        UIMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(UIMode);

        // Disabilita ogni input sul Character
        ForCharacter->DisableInput(PC);
    }
}

// ContainerComponent.cpp
bool UContainerComponent::HasSpaceForParts(const TArray<TSubclassOf<ABaseItem>>& PartClasses) const
{
    int32 FreeSlots = InventorySize - Inventory.Num();
    return FreeSlots >= PartClasses.Num();
}

