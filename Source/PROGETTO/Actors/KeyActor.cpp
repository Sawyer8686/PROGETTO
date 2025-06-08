#include "KeyActor.h"
#include "Components/StaticMeshComponent.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKeyActor::AKeyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Mesh->SetRelativeLocation(FVector::ZeroVector);
    Mesh->SetRelativeRotation(FRotator::ZeroRotator);
    Mesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));

	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));

	KeyID = FName("DefaultKey");
    bCollected = false;

}

// Called when the game starts or when spawned
void AKeyActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AKeyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKeyActor::MainInteract(AActor* Interactor)
{

    if (bCollected)
        return; 

    if (APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(Interactor))
    {
        bool bAdded = Player->AddItemToInventory(this);
        
        if (bAdded)
        {
            bCollected = true;
            Player->AddKey(KeyID);
            Player->ShowNotification(FString::Printf(TEXT("Raccolta chiave: %s"), *KeyID.ToString()), 2.0f);
            SetActorHiddenInGame(true);
            SetActorEnableCollision(false);
            UE_LOG(LogTemp, Warning, TEXT("KeyActor::MainInteract: chiave nascosta con bCollected=TRUE"));
        }
    }

    
}
