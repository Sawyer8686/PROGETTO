#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DoorInteractionWidget.generated.h"

class UButton;
class APlayerController;
class ADoorActor;

/**
 * Widget che mostra i pulsanti “Apri” o “Sblocca” quando si interagisce con una porta.
 */
UCLASS()
class PROGETTO_API UDoorInteractionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Inizializza il widget con la porta e il player controller che l'ha invocato
    UFUNCTION(BlueprintCallable)
    void InitializeWithDoor(ADoorActor* InDoor, APlayerController* InPlayerController);

protected:
    virtual void NativeConstruct() override;

    // Pulsanti bindati via 'BindWidget' (devono esistere nel tuo UMG Designer)
    UPROPERTY(meta = (BindWidget))
    UButton* ButtonOpen;

    UPROPERTY(meta = (BindWidget))
    UButton* ButtonUnlock;

    UPROPERTY(meta = (BindWidget))
    UButton* ButtonExit;

    // Riferimento alla porta che ha invocato il widget
    UPROPERTY()
    ADoorActor* OwningDoor;

    // Il player controller da cui proviene l’interazione
    UPROPERTY()
    APlayerController* OwningPC;

    // Funzioni delegate per il click
    UFUNCTION()
    void OnOpenClicked();

    UFUNCTION()
    void OnUnlockClicked();

    UFUNCTION()
    void OnExitClicked();

    /** Timer per auto?rimozione dopo 5 secondi */
    FTimerHandle AutoRemoveTimerHandle;

    UFUNCTION()
    void AutoRemove();
};

