#include "BauleActor.h"
#include "PROGETTO/Components/ContainerComponent.h"
#include "Components/BoxComponent.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "Kismet/GameplayStatics.h"

ABauleActor::ABauleActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Static Mesh
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

    // Container
    ContainerComponent = CreateDefaultSubobject<UContainerComponent>(TEXT("ContainerComponent"));
}

void ABauleActor::MainInteract(AActor* Interactor)
{
    // Solo il personaggio principale può aprire il baule
    if (APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(Interactor))
    {
        ContainerComponent->ToggleContainer(Player);
    }
}

void ABauleActor::BeginPlay()
{
    Super::BeginPlay();

    // Es. puoi bindare un delegate quando il personaggio entra nel volume,
    // oppure chiamare MainInteract dal sistema di input del Character.
}


