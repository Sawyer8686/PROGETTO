#include "BackpackActor.h"
#include "PROGETTO/PROGETTOCharacter.h"

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
	if (APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(Interactor))
	{
		Player->GiveBackpack(); // Abilita inventario
		Player->DroppedBackpack = nullptr; // Reset riferimento
		Destroy(); // Rimuovi zaino dal mondo
	}
}
