#include "InventoryWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Widgets/ItemDescriptionWidget.h" 
#include "PROGETTO/Widgets/EquipSlotSelectionWidget.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "PROGETTO/Actors/BackPackActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "PROGETTO/Widgets/ItemEntryWidget.h"



void UInventoryWidget::UpdateEquippedDisplay()
{
	if (!OwningCharacter)
		return;

	// Recupero la mappa di slot equipaggiati dal Character
	const TMap<EEquipmentSlot, ABaseItem*>& MapEquip = OwningCharacter->EquippedItemSlots;

	// Per ciascuno dei sei slot, cerco se c’è un item nella mappa;
	// se c’è, ne estraggo il nome, altrimenti metto “–” (o “Nessuno”).
	auto GetNameOrNone = [&](EEquipmentSlot ChosenSlot) -> FString {
		const ABaseItem* const* Found = MapEquip.Find(ChosenSlot);
		if (Found && *Found)
			return (*Found)->ItemName;
		else
			return FString(TEXT("Nessuno"));
		};

	// 1) Head
	{
		FString Name = GetNameOrNone(EEquipmentSlot::Head);
		HeadEquippedText->SetText(FText::FromString(Name));
		HeadUnequipButton->SetVisibility(
			(Name != TEXT("Nessuno")) ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
	// 2) RightHand
	{
		FString Name = GetNameOrNone(EEquipmentSlot::RightHand);
		RightHandEquippedText->SetText(FText::FromString(Name));
		RightHandUnequipButton->SetVisibility(
			(Name != TEXT("Nessuno")) ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
	// 3) LeftHand
	{
		FString Name = GetNameOrNone(EEquipmentSlot::LeftHand);
		LeftHandEquippedText->SetText(FText::FromString(Name));
		LeftHandUnequipButton->SetVisibility(
			(Name != TEXT("Nessuno")) ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
	// 4) Torso
	{
		FString Name = GetNameOrNone(EEquipmentSlot::Torso);
		TorsoEquippedText->SetText(FText::FromString(Name));
		TorsoUnequipButton->SetVisibility(
			(Name != TEXT("Nessuno")) ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
	// 5) RightLeg
	{
		FString Name = GetNameOrNone(EEquipmentSlot::RightLeg);
		RightLegEquippedText->SetText(FText::FromString(Name));
		RightLegUnequipButton->SetVisibility(
			(Name != TEXT("Nessuno")) ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
	// 6) LeftLeg
	{
		FString Name = GetNameOrNone(EEquipmentSlot::LeftLeg);
		LeftLegEquippedText->SetText(FText::FromString(Name));
		LeftLegUnequipButton->SetVisibility(
			(Name != TEXT("Nessuno")) ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
}

void UInventoryWidget::OnCloseButtonClicked()
{
	/*if (!OwningCharacter || !IsVisible())
		return;

	// Chiudo eventuali description aperti
	ClearOpenDescriptions();
	
	SetVisibility(ESlateVisibility::Hidden);

	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		OwningCharacter->EnableInput(PC);
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());

		

		if (OwningCharacter->StatsWidgetInstance)
			OwningCharacter->StatsWidgetInstance->SetVisibility(ESlateVisibility::Visible);

		if (OwningCharacter->BackpackStatusWidgetInstance)
			OwningCharacter->BackpackStatusWidgetInstance->SetVisibility(ESlateVisibility::Visible);*/

	if (!OwningCharacter || !IsVisible())
		return;

	// Close all descriptions
	ClearOpenDescriptions();

	// Hide inventory
	SetVisibility(ESlateVisibility::Hidden);

	if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		OwningCharacter->EnableInput(PC);
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());

		if (OwningCharacter->StatsWidgetInstance)
			OwningCharacter->StatsWidgetInstance->SetVisibility(ESlateVisibility::Visible);

		if (OwningCharacter->BackpackStatusWidgetInstance)
			OwningCharacter->BackpackStatusWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
	
}

void UInventoryWidget::SetMyInventoryItems(const TArray<ABaseItem*>& Items, float CurrentWeight, float MaxWeight)
{
	/*if (!ItemsGrid) return;

	// 1) Raccogli solo i puntatori non-null
	TArray<ABaseItem*> Filtered;
	Filtered.Reserve(Items.Num());
	for (ABaseItem* It : Items)
	{
		if (It)
			Filtered.Add(It);
		else
			UE_LOG(LogTemp, Warning, TEXT("SetMyInventoryItems: trovato nullptr in Inventory!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("SetMyInventoryItems: validi = %d"), Filtered.Num());  // debug

	// 2) Ripulisci la griglia
	ItemsGrid->ClearChildren();

	// 3) Popola fino a MaxInventorySlots
	const int32 Columns = 5;  // o estrai da blueprint
	for (int32 i = 0; i < MaxInventorySlots; ++i)
	{
		UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(this, ItemEntryWidgetClass);
		if (!Entry)
			continue;

		// Associa il parent per i description-popup
		Entry->SetInventoryWidgetParent(this);

		if (i < Filtered.Num())
		{
			// usa Filtered (validi) non Items
			Entry->SetupFromItem(Filtered[i], OwningCharacter);
		}
		else
		{
			Entry->DisplayEmptySlot();
		}

		const int32 Row = i / Columns;
		const int32 Col = i % Columns;
		ItemsGrid->AddChildToUniformGrid(Entry, Row, Col);
	}

	// 4) Aggiorna peso
	if (WeightText)
	{
		WeightText->SetText(FText::FromString(
			FString::Printf(TEXT("Peso: %.1f / %.1f"), CurrentWeight, MaxWeight)
		));
	}*/

	if (!ItemsGrid)
		return;

	// Close any open descriptions before rebuilding
	ClearOpenDescriptions();

	// Filter only valid pointers
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
	if (OwningCharacter)
		OwningCharacter->UnequipItemFromSlot(EEquipmentSlot::Head);
}

void UInventoryWidget::OnRightHandUnequipClicked()
{
	if (OwningCharacter)
		OwningCharacter->UnequipItemFromSlot(EEquipmentSlot::RightHand);
}

void UInventoryWidget::OnLeftHandUnequipClicked()
{
	if (OwningCharacter)
		OwningCharacter->UnequipItemFromSlot(EEquipmentSlot::LeftHand);
}

void UInventoryWidget::OnTorsoUnequipClicked()
{
	if (OwningCharacter)
		OwningCharacter->UnequipItemFromSlot(EEquipmentSlot::Torso);
}

void UInventoryWidget::OnRightLegUnequipClicked()
{
	if (OwningCharacter)
		OwningCharacter->UnequipItemFromSlot(EEquipmentSlot::RightLeg);
}

void UInventoryWidget::OnLeftLegUnequipClicked()
{
	if (OwningCharacter)
		OwningCharacter->UnequipItemFromSlot(EEquipmentSlot::LeftLeg);
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
