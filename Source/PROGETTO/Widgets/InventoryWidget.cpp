#include "InventoryWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Widgets/ItemDescriptionWidget.h" 
#include "PROGETTO/Widgets/EquipSlotSelectionWidget.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "PROGETTO/Actors/BackPackActor.h"
#include "Kismet/GameplayStatics.h"
#include "PROGETTO/Components/InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PROGETTO/Components/InventoryComponent.h"
#include "Components/Button.h"
#include "PROGETTO/Widgets/ItemEntryWidget.h"



void UInventoryWidget::UpdateEquippedDisplay()
{
	if (!OwningCharacter)
		return;

	// 1) Recupera il componente inventario in modo sicuro
	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 2) Prendi la mappa slot?item
	const TMap<EEquipmentSlot, ABaseItem*>& MapEquip = InvComp->EquippedItemSlots;

	// 3) Helper per ottenere il nome dell'item o un placeholder
	auto GetNameOrNone = [&](EEquipmentSlot MySlot) -> FString
		{
			if (const ABaseItem* const* Found = MapEquip.Find(MySlot); Found && *Found)
			{
				return (*Found)->ItemName;
			}
			return FString(TEXT("-")); // o "Nessuno"
		};

	// 4) Helper per aggiornare ciascuno slot (text + button)
	auto UpdateSlot = [&](EEquipmentSlot ASlot, UTextBlock* Txt, UButton* Btn)
		{
			if (!Txt || !Btn)
				return;

			const FString Name = GetNameOrNone(ASlot);
			Txt->SetText(FText::FromString(Name));
			Btn->SetVisibility(Name != TEXT("-")
				? ESlateVisibility::Visible
				: ESlateVisibility::Hidden);
		};

	// 5) Aggiorna tutti e sei gli slot
	UpdateSlot(EEquipmentSlot::Head, HeadEquippedText, HeadUnequipButton);
	UpdateSlot(EEquipmentSlot::RightHand, RightHandEquippedText, RightHandUnequipButton);
	UpdateSlot(EEquipmentSlot::LeftHand, LeftHandEquippedText, LeftHandUnequipButton);
	UpdateSlot(EEquipmentSlot::Torso, TorsoEquippedText, TorsoUnequipButton);
	UpdateSlot(EEquipmentSlot::RightLeg, RightLegEquippedText, RightLegUnequipButton);
	UpdateSlot(EEquipmentSlot::LeftLeg, LeftLegEquippedText, LeftLegUnequipButton);

	
}

void UInventoryWidget::OnCloseButtonClicked()
{
	
	// 1) Controlla OwningCharacter
	if (!OwningCharacter)
		return;

	// 2) Recupera in modo sicuro il componente d’inventario
	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	OwningCharacter->ToggleInventory();

	//if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	//{
		//OwningCharacter->EnableInput(PC);
		//PC->SetShowMouseCursor(false);
		//PC->SetInputMode(FInputModeGameOnly());

		//if (OwningCharacter->StatsWidgetInstance)
			//OwningCharacter->StatsWidgetInstance->SetVisibility(ESlateVisibility::Visible);

	//}
	
}

void UInventoryWidget::SetMyInventoryItems(const TArray<ABaseItem*>& Items, float CurrentWeight, float MaxWeight)
{
	
	/*if (!ItemsGrid)
		return;

	ClearOpenDescriptions();

	TArray<ABaseItem*> Filtered;
	Filtered.Reserve(Items.Num());
	for (ABaseItem* It : Items)
	{
		if (It)
			Filtered.Add(It);
	}

	ItemsGrid->ClearChildren();


	const int32 NumValid = Filtered.Num();
	const int32 Columns = 5;


	for (int32 i = 0; i < MaxInventorySlots; ++i)
	{
		UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(this, ItemEntryWidgetClass);
		if (!Entry)
			continue;

		Entry->SetInventoryWidgetParent(this);

		if (i < NumValid)
		{
			Entry->SetupFromItem(Filtered[i], OwningCharacter);
		}
		else
		{
			Entry->DisplayEmptySlot();
		}

		int32 Row = i / Columns;
		int32 Col = i % Columns;
		ItemsGrid->AddChildToUniformGrid(Entry, Row, Col);
	}

	if (WeightText)
	{
		WeightText->SetText(FText::FromString(
			FString::Printf(TEXT("Peso: %.1f / %.1f"), CurrentWeight, MaxWeight)
		));
	}*/

	if (!ItemsGrid)
		return;

	// Chiudo prima eventuali description-widget aperti
	ClearOpenDescriptions();

	// 1) Filtra solo gli item validi
	TArray<ABaseItem*> Filtered;
	Filtered.Reserve(Items.Num());
	for (ABaseItem* It : Items)
	{
		if (It)
			Filtered.Add(It);
	}

	// 2) Pulisci la griglia
	ItemsGrid->ClearChildren();

	const int32 NumValid = Filtered.Num();

	// 3) Usa Filtered invece di Items e passa sempre il parent
	for (int32 i = 0; i < MaxInventorySlots; ++i)
		
	{
		UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(this, ItemEntryWidgetClass);
		Entry->SetInventoryWidgetParent(this);
		if (!Entry)
			continue;

		        // fondamentale: imposta il parent così SetupFromItem sa a chi registrarsi
			Entry->SetInventoryWidgetParent(this);
		
			if (i < NumValid)
				Entry->SetupFromItem(Filtered[i], OwningCharacter);
			else
				Entry->DisplayEmptySlot();

			int32 Row = i / 5;
			int32 Col = i % 5;
		    ItemsGrid->AddChildToUniformGrid(Entry, Row, Col);
	}

	// 4) Aggiorna il testo del peso
	if (WeightText)
	{
		WeightText->SetText(FText::FromString(
			FString::Printf(TEXT("Peso: %.1f / %.1f"), CurrentWeight, MaxWeight)
		));
	}
}

void UInventoryWidget::RegisterOpenDescription(UItemDescriptionWidget* Description)
{
	if (Description)
	{
		OpenDescriptionWidgets.Add(Description);
	}
}

void UInventoryWidget::UnregisterOpenDescription(UItemDescriptionWidget* Description)
{
	if (Description)
	{
		OpenDescriptionWidgets.RemoveSingle(Description);
	}
}

void UInventoryWidget::ClearOpenDescriptions()
{
	for (UItemDescriptionWidget* Desc : OpenDescriptionWidgets)
	{
		if (Desc)
		{
			Desc->RemoveFromParent();
		}
	}
	OpenDescriptionWidgets.Empty();
}

void UInventoryWidget::NativeConstruct()
{
	
	Super::NativeConstruct();

	// 1) Bind per il CloseButton
	if (CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnCloseButtonClicked);

	if (DropBackpackButton)
	{
		DropBackpackButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnDropBackpackClicked);
	}

	// 3) Bind per i sei bottoni “Unequip” fissi
	if (HeadUnequipButton)
		HeadUnequipButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnHeadUnequipClicked);

	if (RightHandUnequipButton)
		RightHandUnequipButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnRightHandUnequipClicked);

	if (LeftHandUnequipButton)
		LeftHandUnequipButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnLeftHandUnequipClicked);

	if (TorsoUnequipButton)
		TorsoUnequipButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnTorsoUnequipClicked);

	if (RightLegUnequipButton)
		RightLegUnequipButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnRightLegUnequipClicked);

	if (LeftLegUnequipButton)
		LeftLegUnequipButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnLeftLegUnequipClicked);

}

void UInventoryWidget::OnHeadUnequipClicked()
{
	// 1) Sicurezza base
	if (!OwningCharacter)
		return;

	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 2) Rimuovi l’item dallo slot e ricollocalo nell’inventario
	InvComp->UnequipItemFromSlot(EEquipmentSlot::Head);

	// 3) Ricostruisci la lista dell’inventario
	//    (SetMyInventoryItems dovrebbe ricreare le righe col loro pulsante “Equip”)
	SetMyInventoryItems(
		InvComp->Inventory,
		InvComp->CurrentCarryWeight,
		InvComp->MaxCarryWeight
	);

	// 4) Aggiorna la visualizzazione degli slot equipaggiati
	UpdateEquippedDisplay();

	// 5) Assicura che il widget resti visibile
	SetVisibility(ESlateVisibility::Visible);

	// 6) Ripristina modalità UI-only e cursore
	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		// Tieni fermo il personaggio durante l’interazione UI
		OwningCharacter->DisableInput(PC);
	}
}

void UInventoryWidget::OnRightHandUnequipClicked()
{
	// 1) Sicurezza base
	if (!OwningCharacter)
		return;

	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 2) Rimuovi l’item dallo slot e ricollocalo nell’inventario
	InvComp->UnequipItemFromSlot(EEquipmentSlot::RightHand);

	// 3) Ricostruisci la lista dell’inventario
	//    (SetMyInventoryItems dovrebbe ricreare le righe col loro pulsante “Equip”)
	SetMyInventoryItems(
		InvComp->Inventory,
		InvComp->CurrentCarryWeight,
		InvComp->MaxCarryWeight
	);

	// 4) Aggiorna la visualizzazione degli slot equipaggiati
	UpdateEquippedDisplay();

	// 5) Assicura che il widget resti visibile
	SetVisibility(ESlateVisibility::Visible);

	// 6) Ripristina modalità UI-only e cursore
	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		// Tieni fermo il personaggio durante l’interazione UI
		OwningCharacter->DisableInput(PC);
	}
}

void UInventoryWidget::OnLeftHandUnequipClicked()
{
	// 1) Sicurezza base
	if (!OwningCharacter)
		return;

	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 2) Rimuovi l’item dallo slot e ricollocalo nell’inventario
	InvComp->UnequipItemFromSlot(EEquipmentSlot::LeftHand);

	// 3) Ricostruisci la lista dell’inventario
	//    (SetMyInventoryItems dovrebbe ricreare le righe col loro pulsante “Equip”)
	SetMyInventoryItems(
		InvComp->Inventory,
		InvComp->CurrentCarryWeight,
		InvComp->MaxCarryWeight
	);

	// 4) Aggiorna la visualizzazione degli slot equipaggiati
	UpdateEquippedDisplay();

	// 5) Assicura che il widget resti visibile
	SetVisibility(ESlateVisibility::Visible);

	// 6) Ripristina modalità UI-only e cursore
	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		// Tieni fermo il personaggio durante l’interazione UI
		OwningCharacter->DisableInput(PC);
	}
}

void UInventoryWidget::OnTorsoUnequipClicked()
{
	// 1) Sicurezza base
	if (!OwningCharacter)
		return;

	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 2) Rimuovi l’item dallo slot e ricollocalo nell’inventario
	InvComp->UnequipItemFromSlot(EEquipmentSlot::Torso);

	// 3) Ricostruisci la lista dell’inventario
	//    (SetMyInventoryItems dovrebbe ricreare le righe col loro pulsante “Equip”)
	SetMyInventoryItems(
		InvComp->Inventory,
		InvComp->CurrentCarryWeight,
		InvComp->MaxCarryWeight
	);

	// 4) Aggiorna la visualizzazione degli slot equipaggiati
	UpdateEquippedDisplay();

	// 5) Assicura che il widget resti visibile
	SetVisibility(ESlateVisibility::Visible);

	// 6) Ripristina modalità UI-only e cursore
	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		// Tieni fermo il personaggio durante l’interazione UI
		OwningCharacter->DisableInput(PC);
	}
}

void UInventoryWidget::OnRightLegUnequipClicked()
{
	// 1) Sicurezza base
	if (!OwningCharacter)
		return;

	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 2) Rimuovi l’item dallo slot e ricollocalo nell’inventario
	InvComp->UnequipItemFromSlot(EEquipmentSlot::RightLeg);

	// 3) Ricostruisci la lista dell’inventario
	//    (SetMyInventoryItems dovrebbe ricreare le righe col loro pulsante “Equip”)
	SetMyInventoryItems(
		InvComp->Inventory,
		InvComp->CurrentCarryWeight,
		InvComp->MaxCarryWeight
	);

	// 4) Aggiorna la visualizzazione degli slot equipaggiati
	UpdateEquippedDisplay();

	// 5) Assicura che il widget resti visibile
	SetVisibility(ESlateVisibility::Visible);

	// 6) Ripristina modalità UI-only e cursore
	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		// Tieni fermo il personaggio durante l’interazione UI
		OwningCharacter->DisableInput(PC);
	}
}

void UInventoryWidget::OnLeftLegUnequipClicked()
{
	// 1) Sicurezza base
	if (!OwningCharacter)
		return;

	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp)
		return;

	// 2) Rimuovi l’item dallo slot e ricollocalo nell’inventario
	InvComp->UnequipItemFromSlot(EEquipmentSlot::LeftLeg);

	// 3) Ricostruisci la lista dell’inventario
	//    (SetMyInventoryItems dovrebbe ricreare le righe col loro pulsante “Equip”)
	SetMyInventoryItems(
		InvComp->Inventory,
		InvComp->CurrentCarryWeight,
		InvComp->MaxCarryWeight
	);

	// 4) Aggiorna la visualizzazione degli slot equipaggiati
	UpdateEquippedDisplay();

	// 5) Assicura che il widget resti visibile
	SetVisibility(ESlateVisibility::Visible);

	// 6) Ripristina modalità UI-only e cursore
	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		// Tieni fermo il personaggio durante l’interazione UI
		OwningCharacter->DisableInput(PC);
	}
}

void UInventoryWidget::FocusFirstButton()
{
	if (CloseButton && CloseButton->IsVisible() && CloseButton->IsVisible())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(CloseButton->TakeWidget()));
			PC->SetShowMouseCursor(true);
			CloseButton->SetUserFocus(PC);
		}
	}
}

void UInventoryWidget::OnDropBackpackClicked()
{
	// 1) Verifica OwningCharacter
	if (!OwningCharacter)
		return;

	// 2) Recupera il componente in modo sicuro
	UInventoryComponent* InvComp = OwningCharacter->FindComponentByClass<UInventoryComponent>();
	if (!InvComp || !InvComp->bHasBackpack)
		return;

	// 3) Fai drop dello zaino
	InvComp->DropBackpack();

	// 4) Nascondi il widget inventario
	SetVisibility(ESlateVisibility::Hidden);

	// 5) Ripristina input e cursore sul controller del giocatore
	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		PC->SetShowMouseCursor(false);

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		OwningCharacter->EnableInput(PC);
	}
}
