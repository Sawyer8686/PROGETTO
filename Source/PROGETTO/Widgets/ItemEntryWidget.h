
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "Components/Button.h"
#include "EquipSlotSelectionWidget.h"
#include "Components/TextBlock.h"
#include "ItemEntryWidget.generated.h"

class UItemDescriptionWidget;
class APROGETTOCharacter;
class ATorchActor;
class ABatteryActor;
class UInventoryWidget; 

UCLASS()
class PROGETTO_API UItemEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    //VARIABILI E RIFERIMENTI

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equip")
    TSubclassOf<UEquipSlotSelectionWidget> EquipSlotSelectionWidgetClass;

    UEquipSlotSelectionWidget* EquipSlotSelectionWidgetInstance = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DetailsText;

    UPROPERTY(meta = (BindWidget))
    UButton* UseButton;

    UPROPERTY(meta = (BindWidget))
    UButton* DescriptionButton;

    UPROPERTY(meta = (BindWidget))
    UButton* DiscardButton;

    UPROPERTY(meta = (BindWidget))
    UButton* EquipButton;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UItemDescriptionWidget> ItemDescriptionWidgetClass;

    UPROPERTY()
    ABaseItem* Item = nullptr;

    UPROPERTY()
    APROGETTOCharacter* OwningCharacter;

    UPROPERTY()
    ATorchActor* EquippedTorch;

    UInventoryWidget* ParentInventoryWidget;

    UPROPERTY()
    UItemDescriptionWidget* ItemDescriptionInstance;

	//FUNZIONI

    /** Assegna il widget Inventory padre */
    UFUNCTION(BlueprintCallable, Category = "Item Entry")
    void SetParentInventoryWidget(UInventoryWidget* Parent) { ParentInventoryWidget = Parent; }

    void SetInventoryWidgetParent(UInventoryWidget* Parent);

    UFUNCTION()
    void OnUseButtonClicked();

    UFUNCTION()
    void OnDescriptionButtonClicked();

    UFUNCTION()
    void OnDiscardButtonClicked();

    UFUNCTION()
    void OnEquipButtonClicked();

    UFUNCTION(BlueprintCallable)
    void SetupFromItem(ABaseItem* InItem, APROGETTOCharacter* InOwningCharacter);

    UFUNCTION(BlueprintCallable)
    void DisplayEmptySlot();

    virtual void NativeConstruct() override;

    
};

