#include "BackpackActor.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABackpackActor::ABackpackActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Di default, puoi già abilitare la fisica qui (in caso il BP cambiasse l’impostazione):
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

}

// Called when the game starts or when spawned
void ABackpackActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABackpackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABackpackActor::MainInteract(AActor* Interactor)
{
    // 1) Prendi il pawn e cast in APROGETTOCharacter
    APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player)
        return;            // niente character: esci

    // 2) Recupera il componente in modo sicuro
    UInventoryComponent* InvComp = Player->FindComponentByClass<UInventoryComponent>();
    if (!InvComp)
        return;            // niente componente: esci

    // 3) Adesso sei sicuro che Owner e componente sono validi
    InvComp->GiveBackpack();

    // 5) Distruggi lo zaino
    Destroy();
}
