#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PROGETTO/Actors/CompositeItem.h"
#include "CompositePartsWidget.generated.h"

class UVerticalBox;
class UButton;
class APROGETTOCharacter;
//class ACompositeItem;
class UCompositePartEntryWidget;

UCLASS()
class PROGETTO_API UCompositePartsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Il character proprietario, per chiamare DismantleItem */
    UPROPERTY(BlueprintReadWrite, Category = "Composite")
    APROGETTOCharacter* OwningCharacter;

    /** L’item composite di cui mostro le parti */
    UPROPERTY(BlueprintReadWrite, Category = "Composite")
    ACompositeItem* PendingComposite;

    /** Classe del widget entry che mostra nome + icona di ciascuna parte */
    UPROPERTY(EditAnywhere, Category = "Composite")
    TSubclassOf<UCompositePartEntryWidget> CompositePartEntryWidgetClass;

    /** Container dove aggiungere le entry delle parti */
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* PartsListBox;

    /** Bottone conferma/disassembla */
    UPROPERTY(meta = (BindWidget))
    UButton* ConfirmButton;

    /** Bottone annulla */
    UPROPERTY(meta = (BindWidget))
    UButton* CancelButton;

    //UPROPERTY(meta = (BindWidget))
    //UButton* ConfirmButton;

    /** Inizializza il widget (da chiamare subito dopo CreateWidget) */
    UFUNCTION()
    void SetupForComposite(ACompositeItem* InComposite, APROGETTOCharacter* InChar);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnConfirmClicked();

    UFUNCTION()
    void OnCancelClicked();
};

