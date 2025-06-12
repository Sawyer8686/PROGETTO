
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
class UInventoryComponent;
class UCompositePartsWidget;

// Delegate per azioni custom su un item
DECLARE_DELEGATE_OneParam(FOnItemAction, ABaseItem*);

UCLASS()
class PROGETTO_API UItemEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    //VARIABILI E RIFERIMENTI

    /** Bottone Prendi / Take */
    UPROPERTY(meta = (BindWidget))
    UButton* TakeButton;

    /** Bottone Deposita / Store */
    UPROPERTY(meta = (BindWidget))
    UButton* StoreButton;

    /** Delegate chiamato da TakeButton */
    FOnItemAction OnTakeAction;
    /** Delegate chiamato da StoreButton */
    FOnItemAction OnStoreAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Componet")
    UInventoryComponent* Inv;

    
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

    UPROPERTY(meta = (BindWidget))
    UButton * DismantleButton;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UItemDescriptionWidget> ItemDescriptionWidgetClass;

    UPROPERTY()
    ABaseItem* Item = nullptr;

    UPROPERTY()
    APROGETTOCharacter* OwningCharacter;

    UInventoryWidget* ParentInventoryWidget;

    UPROPERTY()
    UItemDescriptionWidget* ItemDescriptionInstance;

    UPROPERTY(EditAnywhere, Category = "Composite")
    TSubclassOf<UCompositePartsWidget> CompositePartsWidgetClass;
    UPROPERTY()
    UCompositePartsWidget* CompositePartsWidgetInstance;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StoreButtonText;

    // e per TakeButton
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TakeButtonText;

    UFUNCTION()
    void HandleTake();

    UFUNCTION()
    void HandleStore();

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

    UFUNCTION()
    void OnDismantleButtonClicked();

    UFUNCTION(BlueprintCallable)
    void SetupFromItem(ABaseItem* InItem, APROGETTOCharacter* InOwningCharacter);

    UFUNCTION(BlueprintCallable)
    void DisplayEmptySlot();

    virtual void NativeConstruct() override;

    
};

