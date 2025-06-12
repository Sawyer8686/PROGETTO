#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChestWidget.generated.h"

class UVerticalBox;
class UButton;
class UItemEntryWidget;
class UContainerComponent;
class UItemEntryWidget;
class APROGETTOCharacter;
class UStaticMeshComponent;
class ABaseItem;

UCLASS()
class PROGETTO_API UChestWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Inizializza il widget con il container e il character */
    UFUNCTION()
    void SetupForContainer(UContainerComponent* InContainer, APROGETTOCharacter* InPlayer);

protected:
    virtual void NativeConstruct() override;

    /** Box per le entry del baule e del giocatore */
    UPROPERTY(meta = (BindWidget)) UVerticalBox* ChestListBox;
    UPROPERTY(meta = (BindWidget)) UVerticalBox* PlayerListBox;

    /** Close del baule */
    UPROPERTY(meta = (BindWidget)) UButton* CloseButton;

private:
    UPROPERTY()
    UContainerComponent* ContainerComp;

    UPROPERTY()
    APROGETTOCharacter* PlayerChar;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UItemEntryWidget> ItemEntryWidgetClass;

    /** Ricostruisce entrambe le liste */
    void RefreshLists();

    UFUNCTION()
    void OnCloseClicked();

    /** Callbacks dalle entry di item */
    void OnTakeFromChest(ABaseItem* Item);
    void OnStoreToChest(ABaseItem* Item);
};

