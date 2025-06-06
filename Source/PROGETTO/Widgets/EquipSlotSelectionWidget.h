#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "EquipSlotSelectionWidget.generated.h"

class UButton;
class ABaseItem;
class APROGETTOCharacter;

UCLASS()
class PROGETTO_API UEquipSlotSelectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // L’item che voglio equipaggiare
    UPROPERTY(BlueprintReadOnly, Category = "Equip")
    ABaseItem* PendingItem = nullptr;


    // Il Character che fa l’equip
    UPROPERTY(BlueprintReadOnly, Category = "Equip")
    APROGETTOCharacter* OwningCharacter = nullptr;

    // Questi bottoni vanno bindati nel Designer
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
};

