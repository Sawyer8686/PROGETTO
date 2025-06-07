
#include "ItemEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Widgets/ItemDescriptionWidget.h"
#include "PROGETTO/Widgets/InventoryWidget.h" 
#include "PROGETTO/Actors/KeyActor.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "PROGETTO/Actors/BatteryActor.h"
#include "Kismet/GameplayStatics.h"
#include "PROGETTO/Actors/TorchActor.h"
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

	// Verifico se è una BatteryActor
	if (ABatteryActor* Battery = Cast<ABatteryActor>(Item))
	{
		// Solo se la batteria corrente è inferiore al massimo posso usare questa batteria
		if (OwningCharacter->CurrentBatteryEnergy >= OwningCharacter->MaxBatteryEnergy)
		{
			OwningCharacter->ShowNotification(TEXT("Batteria già al massimo!"), 1.5f);
			return;
		}

		float Amount = Battery->ChargeAmount;
		// Calcolo nuova carica e uso SetBatteryEnergy (fa già FMath::Clamp internamente)
		OwningCharacter->SetBatteryEnergy(OwningCharacter->CurrentBatteryEnergy + Amount);

		// Rimuovo la batteria dall'inventario e la distruggo
		OwningCharacter->RemoveItemFromInventory(Item);
		if (OwningCharacter->InventoryWidgetInstance &&
			OwningCharacter->InventoryWidgetInstance->IsVisible())
		{
			OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
				OwningCharacter->Inventory,
				OwningCharacter->CurrentCarryWeight,
				OwningCharacter->MaxCarryWeight
			);
		}
		Battery->Destroy();
		return;
	}

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

		OwningCharacter->RemoveItemFromInventory(Item);
		Item->Destroy();
	}

	if (OwningCharacter->InventoryWidgetInstance &&
		OwningCharacter->InventoryWidgetInstance->IsVisible())
	{
		OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
			OwningCharacter->Inventory,
			OwningCharacter->CurrentCarryWeight,
			OwningCharacter->MaxCarryWeight
		);
	}
}

void UItemEntryWidget::OnDescriptionButtonClicked()
{
	
	if (!Item || !ItemDescriptionWidgetClass)
		return;

	// Se il description è già aperto, lo chiudo e resetto
	if (ItemDescriptionInstance && ItemDescriptionInstance->IsInViewport())
	{
		ItemDescriptionInstance->RemoveFromParent();
		if (ParentInventoryWidget)
			ParentInventoryWidget->ClearOpenDescriptions();
		ItemDescriptionInstance = nullptr;
		return;
	}

	// *** APRI NUOVA ISTANZA ***
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		// Usa il PlayerController come context in CreateWidget
		ItemDescriptionInstance = CreateWidget<UItemDescriptionWidget>(PC, ItemDescriptionWidgetClass);
		if (ItemDescriptionInstance)
		{
			// PRIMA aggiungi al viewport
			ItemDescriptionInstance->AddToViewport(101);
			// POI imposta il testo
			ItemDescriptionInstance->SetDescriptionText(Item->Description);
			// Registra nel parent
			if (ParentInventoryWidget)
				ParentInventoryWidget->ClearOpenDescriptions();

			// Ripristino focus su inventario e input mode
			if (APlayerController* APC = UGameplayStatics::GetPlayerController(this, 0))
			{
				if (ParentInventoryWidget && ParentInventoryWidget->CloseButton)
				{
					APC->SetShowMouseCursor(true);
					FInputModeUIOnly UIInput;
					UIInput.SetWidgetToFocus(ParentInventoryWidget->CloseButton->TakeWidget());
					UIInput.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					APC->SetInputMode(UIInput);
				}
			}
		}
		else
		{
			// Assicuro che non ci sia un'istanza rimasta
			if (ItemDescriptionInstance)
			{
				ItemDescriptionInstance->RemoveFromParent();
				ItemDescriptionInstance = nullptr;
			}

			// Apro un nuovo description widget
			ItemDescriptionInstance = CreateWidget<UItemDescriptionWidget>(GetWorld(), ItemDescriptionWidgetClass);
			if (ItemDescriptionInstance)
			{
				ItemDescriptionInstance->SetParentInventoryWidget(ParentInventoryWidget);
				ItemDescriptionInstance->SetDescriptionText(Item->Description);
				ItemDescriptionInstance->AddToViewport(101);
				ItemDescriptionInstance->SetVisibility(ESlateVisibility::Visible);

				if (ParentInventoryWidget)
				{
					ParentInventoryWidget->RegisterOpenDescription(ItemDescriptionInstance);
				}

				// Imposto modalità input per UI e game, focus su description
				if (APlayerController* APC = UGameplayStatics::GetPlayerController(this, 0))
				{
					APC->SetShowMouseCursor(true);
					FInputModeGameAndUI InputMode;
					InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					InputMode.SetWidgetToFocus(ItemDescriptionInstance->TakeWidget());
					APC->SetInputMode(InputMode);
				}
			}
		}


	}

	
	
}

void UItemEntryWidget::OnDiscardButtonClicked()
{
	
	if (!Item || !OwningCharacter)
		return;

	OwningCharacter->DropItem(Item);

	if (OwningCharacter->InventoryWidgetInstance &&
		OwningCharacter->InventoryWidgetInstance->IsVisible())
	{
		OwningCharacter->InventoryWidgetInstance->SetMyInventoryItems(
			OwningCharacter->Inventory,
			OwningCharacter->CurrentCarryWeight,
			OwningCharacter->MaxCarryWeight
		);
	}

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
			OwningCharacter->UnequipItem(Item);
	
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
	










