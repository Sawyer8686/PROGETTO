#include "DoorActor.h"
#include "Components/StaticMeshComponent.h"
#include "PROGETTO/Widgets/DoorInteractionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
ADoorActor::ADoorActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Creo il componente Mesh e lo rendo root
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    RootComponent = DoorMesh;

    // Abilito collision per interazione (ma non la fisica)
    DoorMesh->SetSimulatePhysics(false);
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));

    // Default: nessuna chiave richiesta
    RequiredKeyID = NAME_None;
    OpeningDuration = 1.0f;

    bIsOpen = false;
    bIsOpening = false;
    bIsClosing = false;
    ElapsedTime = 0.0f;

    InteractionWidgetClass = nullptr;
    InteractionWidgetInstance = nullptr;

}

void ADoorActor::MainInteract(AActor* Interactor)
{
    
    if (bIsOpening || bIsClosing)
        return;

    APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(Interactor);
    if (!Player)
        return;

    APlayerController* PC = Cast<APlayerController>(Player->GetController());
    if (!PC)
        return;

    if (bIsOpen)
    {
        StartClosing();
        return;
    }
    else
    {
        ShowInteractionWidget(PC);
    }
}

void ADoorActor::StartOpening()
{
    bIsOpening = true;
    bIsClosing = false;
    ElapsedTime = 0.0f;
    bIsOpen = false; 
}

void ADoorActor::StartClosing()
{
    bIsClosing = true;
    bIsOpening = false;
    ElapsedTime = 0.0f;
    bIsOpen = true; 
}

void ADoorActor::UpdateAnimation(float DeltaSeconds)
{
    ElapsedTime += DeltaSeconds;
    float Alpha = FMath::Clamp(ElapsedTime / OpeningDuration, 0.0f, 1.0f);

    if (bIsOpening)
    {
        
        FRotator NewRot = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
        SetActorRotation(NewRot);

        if (Alpha >= 1.0f)
        {
            
            SetActorRotation(TargetRotation);
            bIsOpening = false;
            bIsOpen = true;
        }
    }
    else if (bIsClosing)
    {
        
        FRotator NewRot = FMath::Lerp(TargetRotation, InitialRotation, Alpha);
        SetActorRotation(NewRot);

        if (Alpha >= 1.0f)
        {
           
            SetActorRotation(InitialRotation);
            bIsClosing = false;
            bIsOpen = false;
        }
    }
}

void ADoorActor::UpdateOpening(float DeltaSeconds)
{
    ElapsedTime += DeltaSeconds;
    float Alpha = FMath::Clamp(ElapsedTime / OpeningDuration, 0.0f, 1.0f);

    
    FRotator NewRot = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
    SetActorRotation(NewRot);

    
    if (Alpha >= 1.0f)
    {
        SetActorRotation(TargetRotation);
    }
}

void ADoorActor::ShowInteractionWidget(APlayerController* PC)
{
    if (!InteractionWidgetClass || !PC)
        return;

    if (InteractionWidgetInstance && InteractionWidgetInstance->IsInViewport())
    {
        InteractionWidgetInstance->RemoveFromParent();
        InteractionWidgetInstance = nullptr;
    }

    InteractionWidgetInstance = CreateWidget<UDoorInteractionWidget>(PC, InteractionWidgetClass);
    if (!InteractionWidgetInstance)
        return;

    InteractionWidgetInstance->AddToViewport();
    InteractionWidgetInstance->InitializeWithDoor(this, PC);
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

    InitialRotation = GetActorRotation();
    TargetRotation = InitialRotation + FRotator(0.f, 90.f, 0.f);

}

void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

   
    if (bIsOpening || bIsClosing)
    {
        UpdateAnimation(DeltaTime);
    }

}
