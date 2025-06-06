#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "Components/VerticalBox.h"
#include "Components/UniformGridPanel.h"
#include "PROGETTO/Widgets/ItemDescriptionWidget.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "InventoryWidget.generated.h"

class APROGETTOCharacter;
class UItemEntryWidget;
class UItemDescriptionWidget; 
class UEquippedItemsWidget;

UCLASS()
class PROGETTO_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// ??? “Unequip” fisso: metodi membro che verranno bindati in NativeConstruct() ???
	UFUNCTION()
	void OnHeadUnequipClicked();
	UFUNCTION()
	void OnRightHandUnequipClicked();
	UFUNCTION()
	void OnLeftHandUnequipClicked();
	UFUNCTION()
	void OnTorsoUnequipClicked();
	UFUNCTION()
	void OnRightLegUnequipClicked();
	UFUNCTION()
	void OnLeftLegUnequipClicked();

	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HeadEquippedText;

	UPROPERTY(meta = (BindWidget))
	UButton* HeadUnequipButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RightHandEquippedText;

	UPROPERTY(meta = (BindWidget))
	UButton* RightHandUnequipButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LeftHandEquippedText;

	UPROPERTY(meta = (BindWidget))
	UButton* LeftHandUnequipButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TorsoEquippedText;

	UPROPERTY(meta = (BindWidget))
	UButton* TorsoUnequipButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RightLegEquippedText;

	UPROPERTY(meta = (BindWidget))
	UButton* RightLegUnequipButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LeftLegEquippedText;

	UPROPERTY(meta = (BindWidget))
	UButton* LeftLegUnequipButton;

	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateEquippedDisplay();

	void SetMyInventoryItems(const TArray<ABaseItem*>& Items, float CurrentWeight, float MaxWeight);

	void RegisterDescriptionWidget(UItemDescriptionWidget* DescWidget);
	void UnregisterDescriptionWidget(UItemDescriptionWidget* DescWidget);

	UPROPERTY()
	TArray<UItemDescriptionWidget*> OpenDescriptionWidgets;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ItemsBox;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* ItemsGrid;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 MaxInventorySlots = 5;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeightText;

	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;

	UFUNCTION()
	void OnCloseButtonClicked();

	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UItemEntryWidget> ItemEntryWidgetClass;

	UPROPERTY()
	class APROGETTOCharacter* OwningCharacter;

	/** Inizializza il widget con riferimento al personaggio proprietario */
	void SetOwningCharacter(APROGETTOCharacter* Character) { OwningCharacter = Character; }

	void FocusFirstButton();
};
