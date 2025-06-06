#include "MySafeZone.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Widgets/SafeZoneMessageWidget.h"

// Sets default values
AMySafeZone::AMySafeZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMySafeZone::OnOverlapBegin);
    BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AMySafeZone::OnOverlapEnd);

    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false;

}

void AMySafeZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Cast<APROGETTOCharacter>(OtherActor))
    {
        AudioComponent->FadeIn(1.5f); // Fai partire lentamente la musica
        ShowSafeZoneMessage();
        UE_LOG(LogTemp, Warning, TEXT("Entrato nella SafeZone: musica avviata"));
    }
}

void AMySafeZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (Cast<APROGETTOCharacter>(OtherActor))
    {
        AudioComponent->FadeOut(1.5f, 0.0f); // Ferma con dissolvenza
        //AudioComponent->Stop();
        UE_LOG(LogTemp, Warning, TEXT("Uscito dalla SafeZone: musica fermata"));
    }
}

void AMySafeZone::ShowSafeZoneMessage()
{
    if (UWorld* World = GetWorld())
    {
        if (!SafeZoneMessageWidgetInstance)
        {
            if (SafeZoneMessageWidgetClass)
            {
                // Crea il widget usando la variabile di classe
                SafeZoneMessageWidgetInstance = CreateWidget<USafeZoneMessageWidget>(World, SafeZoneMessageWidgetClass);
                if (SafeZoneMessageWidgetInstance)
                {
                    SafeZoneMessageWidgetInstance->AddToViewport();
                    SafeZoneMessageWidgetInstance->ShowMessage();
                    UE_LOG(LogTemp, Warning, TEXT("Messaggio aggiunto alla viewport"));

                    // Timer per rimuovere il messaggio dopo 3 secondi
                    FTimerHandle TimerHandle;
                    World->GetTimerManager().SetTimer(TimerHandle, this, &AMySafeZone::RemoveSafeZoneMessage, 3.0f, false);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("SafeZoneMessageWidgetClass non è impostato!"));
            }
        }
    }
}

void AMySafeZone::RemoveSafeZoneMessage()
{
    if (SafeZoneMessageWidgetInstance)
    {
        SafeZoneMessageWidgetInstance->RemoveFromViewport();
        SafeZoneMessageWidgetInstance = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("Messaggio rimosso"));
    }
}

void AMySafeZone::BeginPlay()
{
	Super::BeginPlay();

}

void AMySafeZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
