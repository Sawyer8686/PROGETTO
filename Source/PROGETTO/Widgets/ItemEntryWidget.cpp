
#include "ItemEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Widgets/ItemDescriptionWidget.h"
#include "PROGETTO/Widgets/InventoryWidget.h" 
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Texture2D.h"


void UItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UseButton)
	{
		UseButton->OnClicked.AddDynamic(this, &UItemEntryWidget::OnUseButtonClicked);
	}
	if (DescriptionButton)
	{
		DescriptionButton->OnClicked.AddDynamic(this, &UItemEntryWidget::OnDescriptionButtonClicked);
	}
	if (DiscardButton)
	{
		DiscardButton->OnClicked.AddDynamic(this, &UItemEntryWidget::OnDiscardButtonClicked);
	}
	if (EquipButton)
	{
		EquipButton->OnClicked.AddDynamic(this, &UItemEntryWidget::OnEquipButtonClicked);
	}
}

void UItemEntryWidget::SetupFromItem(ABaseItem* InItem, APROGETTOCharacter* InOwningCharacter)
{
	Item = InItem;
	OwningCharacter = InOwningCharacter;

	if (!Item || !DetailsText)
		return;

	FString Info = FString::Printf(TEXT(" %s\n Peso: %.1f"), *Item->ItemName, Item->Weight);
		
	DetailsText->SetText(FText::FromString(Info));

	if (UseButton)
	{
		bool bIsToolOrConsumable = (Item->bCanBeConsumed || Item->ItemType == EItemType::Tool);
		UseButton->SetVisibility(bIsToolOrConsumable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);		
	}

	if (DescriptionButton)
	{
		bool bCanShowDescription = (Item->Description.IsEmpty() == false);
		DescriptionButton->SetVisibility(bCanShowDescription ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);		
	}

	if (DiscardButton)
	{
		DiscardButton->SetVisibility(ESlateVisibility::Visible);
	}

	// Se è equipaggiabile, mostro EquipButton; altrimenti lo nascondo
	if (EquipButton)
	{
		if (Item->ItemType == EItemType::Equippable)
			EquipButton->SetVisibility(ESlateVisibility::Visible);
		else
			EquipButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UItemEntryWidget::DisplayEmptySlot()
{
	if (DetailsText)
	{
		DetailsText->SetText(FText::FromString(TEXT("Slot")));
	}
	if (UseButton)       UseButton->SetVisibility(ESlateVisibility::Collapsed);
	if (DescriptionButton) DescriptionButton->SetVisibility(ESlateVisibility::Collapsed);
	if (DiscardButton)   DiscardButton->SetVisibility(ESlateVisibility::Collapsed);
	if (EquipButton)     EquipButton->SetVisibility(ESlateVisibility::Collapsed);

	Item = nullptr;
	OwningCharacter = nullptr;
}

void UItemEntryWidget::SetInventoryWidgetParent(UInventoryWidget* Parent)
{
	ParentInventoryWidget = Parent;
}

void UItemEntryWidget::OnUseButtonClicked()
{
	if (!Item || !OwningCharacter)
		return;

	if (Item->bCanBeConsumed)
	{
		
		if (Item->ItemType == EItemType::Food)
		{
			OwningCharacter->RestoreHunger(Item->HungerRestore);
		}
		else if (Item->ItemType == EItemType::Beverage)
		{
			OwningCharacter->RestoreThirst(Item->ThirstRestore);
		}

		Inv->RemoveItemFromInventory(Item);
		Item->Destroy();
	}

	if (OwningCharacter->InventoryWidgetInstance &&
		OwningCharacter->InventoryWidgetInstance->IsVisible())
	{
		OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
			Inv->Inventory,
			Inv->CurrentCarryWeight,
			Inv->MaxCarryWeight
		);
	}
}

void UItemEntryWidget::OnDescriptionButtonClicked()
{
	
	if (!ItemDescriptionWidgetClass || !ParentInventoryWidget || !Item)
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
		return;

	// Se il description è già aperto, chiudilo
	if (ItemDescriptionInstance && ItemDescriptionInstance->IsInViewport())
	{
		ItemDescriptionInstance->RemoveFromParent();
		ParentInventoryWidget->UnregisterOpenDescription(ItemDescriptionInstance);
		ParentInventoryWidget->SetIsEnabled(true);

		// Ripristina focus all'inventario
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly UIInput;
		UIInput.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		UIInput.SetWidgetToFocus(ParentInventoryWidget->TakeWidget());
		PC->SetInputMode(UIInput);

		ItemDescriptionInstance = nullptr;
		return;
	}

	// Crea un nuovo description widget
	ItemDescriptionInstance = CreateWidget<UItemDescriptionWidget>(PC, ItemDescriptionWidgetClass);
	if (!ItemDescriptionInstance)
		return;

	// Imposta parent e registra
	ItemDescriptionInstance->SetParentInventoryWidget(ParentInventoryWidget);
	ParentInventoryWidget->RegisterOpenDescription(ItemDescriptionInstance);
	ParentInventoryWidget->SetIsEnabled(false);

	// Mostra il widget
	ItemDescriptionInstance->AddToViewport(101);

	// Imposta il testo della descrizione dall'oggetto
	ItemDescriptionInstance->SetDescriptionText(Item->Description);

	// Focalizza solo il CloseButton
	if (UButton* CloseBtn = ItemDescriptionInstance->GetCloseButton())
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly UIInput;
		UIInput.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		UIInput.SetWidgetToFocus(CloseBtn->TakeWidget());
		PC->SetInputMode(UIInput);
	}
}

void UItemEntryWidget::OnDiscardButtonClicked()
{
	
	// 1) Assicurati di avere un OwningCharacter valido
	if (!OwningCharacter)
		return;

	// 2) Recupera il componente inventario in modo sicuro
	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 3) Verifica che l'item assegnato a questo widget esista
	if (!Item)  // o qualunque membro tenga l'item
		return;

	// 4) Invoca il drop
	InvComp->DropItem(Item);

	// 5) (Opzionale) Rimuovi o aggiorna il widget entry
	RemoveFromParent();

}

void UItemEntryWidget::OnEquipButtonClicked()
{

	UE_LOG(LogTemp, Warning, TEXT("OnEquipButtonClicked() chiamato per item %s"), Item ? *Item->ItemName : TEXT("nessuno"));
	
	if (!Item || !OwningCharacter)
		return;

	// Se è equipaggiabile:
	if (Item->ItemType == EItemType::Equippable)
	{
		// 1) Se è già equip, chiamo semplicemente l’Unequip del Character
		if (Item->bIsEquipped)
		{
			Inv->UnequipItem(Item);
	
			return;
		}

		// 2) Se non è equip e voglio equip, apro il widget di selezione slot
		if (!EquipSlotSelectionWidgetInstance && EquipSlotSelectionWidgetClass)
		{
			EquipSlotSelectionWidgetInstance = CreateWidget<UEquipSlotSelectionWidget>(GetWorld(), EquipSlotSelectionWidgetClass);
			if (EquipSlotSelectionWidgetInstance)
			{
				// Passo il Character e l’Item corrente al widget
				EquipSlotSelectionWidgetInstance->OwningCharacter = OwningCharacter;
				EquipSlotSelectionWidgetInstance->PendingItem = Item;

				// Imposto l’item e il Character nel widget (essenziale per disabilitare/abilitare bottoni)
				EquipSlotSelectionWidgetInstance->SetupForItem(Item, OwningCharacter);

				// Lo mostro con Z-order alto
				EquipSlotSelectionWidgetInstance->AddToViewport(1000);
			}
		}
		// Se esiste già una istanza aperta (non chiusa), potresti ignorare o force-close qui
	}

}
	










