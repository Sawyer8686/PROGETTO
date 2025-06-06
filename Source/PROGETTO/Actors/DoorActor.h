#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Interfaces/MainInteractionInterface.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "DoorActor.generated.h"

class UDoorInteractionWidget;

UCLASS()
class ADoorActor : public AActor, public IMainInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoorActor();

    /** Componente mesh della porta */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
    UStaticMeshComponent* DoorMesh;

    /** Chiave richiesta per aprire la porta */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    FName RequiredKeyID;

    /** Stato aperto/chiuso */
    bool bIsOpen;

    /** Durata animazione apertura (in secondi) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    float OpeningDuration;

    /** Tempo trascorso da inizio animazione di apertura */
    float ElapsedTime;

    /** Angolo di rotazione iniziale (salvato in BeginPlay) */
    FRotator InitialRotation;

    /** Rotazione target quando è completamente aperta (ad es. yaw + 90 gradi) */
    FRotator TargetRotation;

    /** Indica se stiamo in fase di animazione di apertura */
    bool bIsOpening;

    /** Indica se stiamo in fase di animazione di chiusura */
    bool bIsClosing;

    /** Implementazione di IMainInteractionInterface */
    virtual void MainInteract(AActor* Interactor) override;

    /** Inizia la sequenza di apertura */
    void StartOpening();

    /** Avvia l'animazione di chiusura */
    void StartClosing();

    /** Aggiorna l'animazione (sia apertura che chiusura) */
    void UpdateAnimation(float DeltaSeconds);

    /** Aggiorna la rotazione della porta durante Tick se sta aprendo */
    void UpdateOpening(float DeltaSeconds);

    /** Classe del widget di interazione (Blueprint derivato da UDoorInteractionWidget) */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDoorInteractionWidget> InteractionWidgetClass;

    /** Istanza del widget di interazione, se attualmente visibile */
    UPROPERTY()
    UDoorInteractionWidget* InteractionWidgetInstance;

    /** Mostra il widget di interazione sullo schermo del player */
    void ShowInteractionWidget(APlayerController* PC);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
