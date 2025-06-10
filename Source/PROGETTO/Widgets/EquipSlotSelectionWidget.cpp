#include "EquipSlotSelectionWidget.h"
#include "Components/Button.h"
#include "PROGETTO/Widgets/InventoryWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "GameFramework/PlayerController.h"




void UEquipSlotSelectionWidget::SetupForItem(ABaseItem* Item, APROGETTOCharacter* Character)
{
    if (!Item || !Character) return;

    PendingItem = Item;
    OwningCharacter = Character;

    ParentInventory = Character->InventoryWidgetInstance;
    if (!ParentInventory) return;

    // Disable inventory behind
    DisableInventoryBehind();

    // Disable all buttons
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

    // Mostra il modal
    MyShowModal();
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
    // 1) Verifica OwningCharacter
    if (!OwningCharacter)
        return;

    // 2) Recupera il componente inventario in modo sicuro
    UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;

    // 3) Se c'è un item in pendente, prova a equipaggiarlo
    if (PendingItem)
    {
        InvComp->EquipItemToSlot(PendingItem, EEquipmentSlot::Head);
    }

    // 4) Aggiorna il widget di inventario tramite il componente
    if (UInventoryWidget* InvWidget = InvComp->InventoryWidgetInstance)
    {
        InvWidget->SetMyInventoryItems(
            InvComp->Inventory,
            InvComp->CurrentCarryWeight,
            InvComp->MaxCarryWeight
        );
    }

    // 5) Chiudi il popup e ripristina l'inventario alle spalle
    RemoveFromParent();
    EnableInventoryBehind();
}

void UEquipSlotSelectionWidget::OnRightHandClicked()
{
    // 1) Verifica OwningCharacter
    if (!OwningCharacter)
        return;

    // 2) Recupera il componente inventario in modo sicuro
    UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;

    // 3) Se c'è un item in pendente, prova a equipaggiarlo
    if (PendingItem)
    {
        InvComp->EquipItemToSlot(PendingItem, EEquipmentSlot::RightHand);
    }

    // 4) Aggiorna il widget di inventario tramite il componente
    if (UInventoryWidget* InvWidget = InvComp->InventoryWidgetInstance)
    {
        InvWidget->SetMyInventoryItems(
            InvComp->Inventory,
            InvComp->CurrentCarryWeight,
            InvComp->MaxCarryWeight
        );
    }

    // 5) Chiudi il popup e ripristina l'inventario alle spalle
    RemoveFromParent();
    EnableInventoryBehind();
}

void UEquipSlotSelectionWidget::OnLeftHandClicked()
{
    // 1) Verifica OwningCharacter
    if (!OwningCharacter)
        return;

    // 2) Recupera il componente inventario in modo sicuro
    UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;

    // 3) Se c'è un item in pendente, prova a equipaggiarlo
    if (PendingItem)
    {
        InvComp->EquipItemToSlot(PendingItem, EEquipmentSlot::LeftHand);
    }

    // 4) Aggiorna il widget di inventario tramite il componente
    if (UInventoryWidget* InvWidget = InvComp->InventoryWidgetInstance)
    {
        InvWidget->SetMyInventoryItems(
            InvComp->Inventory,
            InvComp->CurrentCarryWeight,
            InvComp->MaxCarryWeight
        );
    }

    // 5) Chiudi il popup e ripristina l'inventario alle spalle
    RemoveFromParent();
    EnableInventoryBehind();
}

void UEquipSlotSelectionWidget::OnTorsoClicked()
{
    // 1) Verifica OwningCharacter
    if (!OwningCharacter)
        return;

    // 2) Recupera il componente inventario in modo sicuro
    UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;

    // 3) Se c'è un item in pendente, prova a equipaggiarlo
    if (PendingItem)
    {
        InvComp->EquipItemToSlot(PendingItem, EEquipmentSlot::Torso);
    }

    // 4) Aggiorna il widget di inventario tramite il componente
    if (UInventoryWidget* InvWidget = InvComp->InventoryWidgetInstance)
    {
        InvWidget->SetMyInventoryItems(
            InvComp->Inventory,
            InvComp->CurrentCarryWeight,
            InvComp->MaxCarryWeight
        );
    }

    // 5) Chiudi il popup e ripristina l'inventario alle spalle
    RemoveFromParent();
    EnableInventoryBehind();
}

void UEquipSlotSelectionWidget::OnRightLegClicked()
{
    // 1) Verifica OwningCharacter
    if (!OwningCharacter)
        return;

    // 2) Recupera il componente inventario in modo sicuro
    UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;

    // 3) Se c'è un item in pendente, prova a equipaggiarlo
    if (PendingItem)
    {
        InvComp->EquipItemToSlot(PendingItem, EEquipmentSlot::RightLeg);
    }

    // 4) Aggiorna il widget di inventario tramite il componente
    if (UInventoryWidget* InvWidget = InvComp->InventoryWidgetInstance)
    {
        InvWidget->SetMyInventoryItems(
            InvComp->Inventory,
            InvComp->CurrentCarryWeight,
            InvComp->MaxCarryWeight
        );
    }

    // 5) Chiudi il popup e ripristina l'inventario alle spalle
    RemoveFromParent();
    EnableInventoryBehind();
}

void UEquipSlotSelectionWidget::OnLeftLegClicked()
{
    // 1) Verifica OwningCharacter
    if (!OwningCharacter)
        return;

    // 2) Recupera il componente inventario in modo sicuro
    UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;

    // 3) Se c'è un item in pendente, prova a equipaggiarlo
    if (PendingItem)
    {
        InvComp->EquipItemToSlot(PendingItem, EEquipmentSlot::LeftLeg);
    }

    // 4) Aggiorna il widget di inventario tramite il componente
    if (UInventoryWidget* InvWidget = InvComp->InventoryWidgetInstance)
    {
        InvWidget->SetMyInventoryItems(
            InvComp->Inventory,
            InvComp->CurrentCarryWeight,
            InvComp->MaxCarryWeight
        );
    }

    // 5) Chiudi il popup e ripristina l'inventario alle spalle
    RemoveFromParent();
    EnableInventoryBehind();
}

void UEquipSlotSelectionWidget::DisableInventoryBehind()
{
    if (OwningCharacter && OwningCharacter->InventoryWidgetInstance)
    {
        OwningCharacter->InventoryWidgetInstance->SetIsEnabled(false);
    }
}

void UEquipSlotSelectionWidget::EnableInventoryBehind()
{
    // 1) Controlla OwningCharacter
    if (!OwningCharacter)
        return;

    // 2) Recupera il componente inventario in modo sicuro
    UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;

    // 3) Recupera l’istanza del widget inventario
    UInventoryWidget* InvWidget = InvComp->InventoryWidgetInstance;
    if (!InvWidget)
        return;

    // 4) Assicurati che il widget sia già aggiunto al viewport
    //    se per qualche motivo non lo fosse, aggiungilo
    if (!InvWidget->IsInViewport())
    {
        InvWidget->AddToViewport(100);
    }

    // 5) Riporta in vista il widget di inventario
    InvWidget->SetVisibility(ESlateVisibility::Visible);

    // 6) Ripristina input e cursore
    if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
    {
        PC->SetShowMouseCursor(true);

        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(InvWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);

        // Disabilita input di gioco sul Character
        OwningCharacter->DisableInput(PC);
    }
}

void UEquipSlotSelectionWidget::HandleSlotSelection(EEquipmentSlot ASlot)
{
    if (OwningCharacter && PendingItem)
    {
        InventoryComponent->EquipItemToSlot(PendingItem, ASlot);
        if (OwningCharacter->InventoryWidgetInstance)
        {
            OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
                InventoryComponent->Inventory,
                InventoryComponent->CurrentCarryWeight,
                InventoryComponent->MaxCarryWeight
            );
        }
    }
    CloseModal();
    
}

void UEquipSlotSelectionWidget::MyShowModal()
{
    AddToViewport(250);
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->SetShowMouseCursor(true);
        FInputModeUIOnly Mode;
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        Mode.SetWidgetToFocus(TakeWidget());
        PC->SetInputMode(Mode);
    }
}

void UEquipSlotSelectionWidget::CloseModal()
{
    EnableInventoryBehind();
    RemoveFromParent();
    if (OwningCharacter && OwningCharacter->InventoryWidgetInstance)
    {
        if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
        {
            PC->SetShowMouseCursor(true);
            FInputModeUIOnly Mode;
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            Mode.SetWidgetToFocus(OwningCharacter->InventoryWidgetInstance->TakeWidget());
            PC->SetInputMode(Mode);
        }
    }
}


