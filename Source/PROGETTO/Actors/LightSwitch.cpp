#include "LightSwitch.h"
#include "PROGETTO/PROGETTOCharacter.h"

// Sets default values
ALightSwitch::ALightSwitch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void ALightSwitch::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALightSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALightSwitch::TogglePower_Implementation()
{
	if (ConnectedLight)
	{
		ConnectedLight->ToggleLight();
	}
}


