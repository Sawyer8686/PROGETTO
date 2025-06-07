#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "EquipSlotSelectionWidget.generated.h"

class UButton;
class ABaseItem;
class APROGETTOCharacter;
class UInventoryWidget;

UCLASS()
class PROGETTO_API UEquipSlotSelectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:

   
    UPROPERTY(BlueprintReadOnly, Category = "Equip")
    ABaseItem* PendingItem = nullptr;
    UPROPERTY(BlueprintReadOnly, Category = "Equip")
    APROGETTOCharacter* OwningCharacter = nullptr;
    UPROPERTY() 
    UInventoryWidget* ParentInventory = nullptr;


    UPROPERTY(meta = (BindWidget))
    UButton* HeadButton;
    UPROPERTY(meta = (BindWidget))
    UButton* RightHandButton;
    UPROPERTY(meta = (BindWidget))
    UButton* LeftHandButton;
    UPROPERTY(meta = (BindWidget))
    UButton* TorsoButton;
    UPROPERTY(meta = (BindWidget))
    UButton* RightLegButton;
    UPROPERTY(meta = (BindWidget))
    UButton* LeftLegButton;

    UFUNCTION(BlueprintCallable, Category = "Equip")
    void SetupForItem(ABaseItem* Item, APROGETTOCharacter* Character);

    virtual void NativeConstruct() override;


    /** Equipment slot button handlers */

    UFUNCTION()
    void OnHeadClicked();
    UFUNCTION()
    void OnRightHandClicked();
    UFUNCTION()
    void OnLeftHandClicked();
    UFUNCTION()
    void OnTorsoClicked();
    UFUNCTION()
    void OnRightLegClicked();
    UFUNCTION()
    void OnLeftLegClicked();


    /** Disabilita i bottoni e l'interazione con la UI di inventory dietro */
    void DisableInventoryBehind();

    /** Riabilita la UI di inventory */
    void EnableInventoryBehind();

    /** Gestisce il click su uno slot */
    void HandleSlotSelection(EEquipmentSlot Slot);

    void MyShowModal();

    void CloseModal();
};


