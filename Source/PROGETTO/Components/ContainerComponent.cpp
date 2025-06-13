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

void UContainerComponent::ToggleContainer(APROGETTOCharacter* Player)
{
    if (!ChestWidgetClass || !Player) return;

    APlayerController* PC = Cast<APlayerController>(Player->GetController());
    if (!PC) return;

    // Se non esiste ancora il widget, lo creo e lo apro
    if (!ChestWidgetInstance)
    {
        // 1) Creo il widget con PC come Outer
        ChestWidgetInstance = CreateWidget<UChestWidget>(PC, ChestWidgetClass);
        if (!ChestWidgetInstance)
        {
            UE_LOG(LogTemp, Error, TEXT("ToggleContainer: impossible to create ChestWidget"));
            return;
        }

        // 2) Chiamo subito Setup e RefreshLists
        ChestWidgetInstance->SetupForContainer(this, Player);

        // 3) Mostro in viewport
        ChestWidgetInstance->AddToViewport(500);

        // 4) Imposto input mode/UI
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
        FInputModeUIOnly UI;
        UI.SetWidgetToFocus(ChestWidgetInstance->TakeWidget()); // o simile
        //UIInput.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(UI);
    }
    else
    {
        // Se il widget esiste, lo chiudo
        ChestWidgetInstance->RemoveFromParent();
        ChestWidgetInstance = nullptr;

        // Ripristino input di gioco
        PC->bShowMouseCursor = false;
        FInputModeGameOnly GameInput;
        PC->SetInputMode(GameInput);
    }
}

// ContainerComponent.cpp
bool UContainerComponent::HasSpaceForParts(const TArray<TSubclassOf<ABaseItem>>& PartClasses) const
{
    int32 FreeSlots = InventorySize - Inventory.Num();
    return FreeSlots >= PartClasses.Num();
}

