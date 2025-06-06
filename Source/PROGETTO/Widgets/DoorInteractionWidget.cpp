#include "DoorInteractionWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Actors/DoorActor.h"

void UDoorInteractionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ButtonOpen)
    {
        ButtonOpen->OnClicked.AddDynamic(this, &UDoorInteractionWidget::OnOpenClicked);
    }
    if (ButtonUnlock)
    {
        ButtonUnlock->OnClicked.AddDynamic(this, &UDoorInteractionWidget::OnUnlockClicked);
    }
    if (ButtonExit)
    {
        ButtonExit->OnClicked.AddDynamic(this, &UDoorInteractionWidget::OnExitClicked);
        
    }
    
}

void UDoorInteractionWidget::InitializeWithDoor(ADoorActor* InDoor, APlayerController* InPlayerController)
{
    OwningDoor = InDoor;
    OwningPC = InPlayerController;

    if (!OwningDoor || !OwningPC)
        return;

    // 1) Mostro il widget in viewport
    AddToViewport();

    // 2) Imposto il focus sul widget e passo in UI-only mode
    OwningPC->SetShowMouseCursor(true);
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    OwningPC->SetInputMode(InputMode);

    // 3) Configuro visibilità dei pulsanti
    if (OwningDoor->RequiredKeyID == NAME_None)
    {
        if (ButtonOpen)
            ButtonOpen->SetVisibility(ESlateVisibility::Visible);
        if (ButtonUnlock)
            ButtonUnlock->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        if (ButtonOpen)
            ButtonOpen->SetVisibility(ESlateVisibility::Collapsed);
        if (ButtonUnlock)
            ButtonUnlock->SetVisibility(ESlateVisibility::Visible);
    }

    // 4) Avvio un timer di 5 secondi per auto-rimozione
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            AutoRemoveTimerHandle,
            this,
            &UDoorInteractionWidget::AutoRemove,
            5.0f,   // dopo 5 secondi
            false   // non looping
        );
    }
}

void UDoorInteractionWidget::OnOpenClicked()
{
    if (!OwningDoor)
        return;

    OwningDoor->StartOpening();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoRemoveTimerHandle);
    }

    OwningPC->SetShowMouseCursor(false);
    OwningPC->SetInputMode(FInputModeGameOnly());

    RemoveFromParent();

    if (OwningDoor && OwningDoor->InteractionWidgetInstance == this)
    {
        OwningDoor->InteractionWidgetInstance = nullptr;
    }
}

void UDoorInteractionWidget::OnUnlockClicked()
{
    if (!OwningDoor || !OwningPC)
        return;

    APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(OwningPC->GetPawn());
    if (!Player)
        return;

    if (Player->HasKey(OwningDoor->RequiredKeyID))
    {
        OwningDoor->StartOpening();
        
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(AutoRemoveTimerHandle);
        }
        
        OwningPC->SetShowMouseCursor(false);
        OwningPC->SetInputMode(FInputModeGameOnly());

        RemoveFromParent();

        if (OwningDoor && OwningDoor->InteractionWidgetInstance == this)
        {
            OwningDoor->InteractionWidgetInstance = nullptr;
        }
    }
    else
    {
        Player->ShowNotification(
            FString::Printf(TEXT("Serve la chiave %s per sbloccare questa porta"), *OwningDoor->RequiredKeyID.ToString()),
            2.0f
        );
    }
}

void UDoorInteractionWidget::OnExitClicked()
{
    
    if (!OwningDoor || !OwningPC)
        return;

    // 1) Cancello il timer (se non è ancora scattato)
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoRemoveTimerHandle);
    }

    // 2) Ripristino input in Game-only
    OwningPC->SetShowMouseCursor(false);
    OwningPC->SetInputMode(FInputModeGameOnly());

    // 3) Rimuovo il widget
    RemoveFromParent();

    // 4) Pulisco il puntatore in DoorActor
    if (OwningDoor && OwningDoor->InteractionWidgetInstance == this)
    {
        OwningDoor->InteractionWidgetInstance = nullptr;
    }
}

void UDoorInteractionWidget::AutoRemove()
{
    if (!OwningPC)
        return;

    // 1) Ripristino input in Game-only
    OwningPC->SetShowMouseCursor(false);
    OwningPC->SetInputMode(FInputModeGameOnly());

    // Rimuovo il widget dalla viewport
    RemoveFromParent();

    // Se OwningDoor punta a questo widget, azzero
    if (OwningDoor && OwningDoor->InteractionWidgetInstance == this)
    {
        OwningDoor->InteractionWidgetInstance = nullptr;
    }
}
