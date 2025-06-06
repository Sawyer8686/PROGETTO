#include "ConsumerEnergyActor.h"
#include "PROGETTO/Actors/SolarPanelActor.h"
#include "TimerManager.h"
#include "LightSource.h"


// Sets default values
AConsumerEnergyActor::AConsumerEnergyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Crea e imposta la mesh come RootComponent
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

}

// Called when the game starts or when spawned
void AConsumerEnergyActor::BeginPlay()
{
	Super::BeginPlay();

	if (ConnectedSolarPanel)
	{
		ConnectedSolarPanel->OnEnergyProduced.AddDynamic(this, &AConsumerEnergyActor::OnSolarEnergyProduced);
	}

	if (ConnectedSolarPanel)
	{
		ConnectedSolarPanel->OnEnergyProduced.AddDynamic(this, &AConsumerEnergyActor::OnSolarEnergyProduced);
		ConnectedSolarPanel->RegisterConsumer(this);
	}

}

// Called every frame
void AConsumerEnergyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPoweredOn)
	{
		ConsumeEnergy();
	}

}

void AConsumerEnergyActor::TogglePower_Implementation()

{
	
	// Se sto cercando di accendere (cioè se era spento)
	if (!bIsPoweredOn)
	{
		// Blocca accensione se il pannello sta producendo energia (es. ricarica)
		if (ConnectedSolarPanel && ConnectedSolarPanel->bIsProducingEnergy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Accensione bloccata: il pannello è in fase di produzione."));
			return; // non accendo
		}
	}

	// Toggle stato power
	bIsPoweredOn = !bIsPoweredOn;

	if (bIsPoweredOn)
	{
		UE_LOG(LogTemp, Log, TEXT("Consumer acceso."));
		GetWorldTimerManager().SetTimer(ConsumptionTimer, this, &AConsumerEnergyActor::ConsumeEnergy, EnergyConsumptionInterval, true);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Consumer spento."));
		GetWorldTimerManager().ClearTimer(ConsumptionTimer);
	}
}


void AConsumerEnergyActor::ConsumeEnergy()
{
	if (!ConnectedSolarPanel)
		return;

	float& CurrentEnergy = ConnectedSolarPanel->CurrentEnergy;

	if (CurrentEnergy >= EnergyConsumptionPerCycle)
	{
		// Consuma normalmente
		CurrentEnergy -= EnergyConsumptionPerCycle;
		UE_LOG(LogTemp, Log, TEXT("Energia consumata: %.2f rimanente."), CurrentEnergy);
	}
	else if (CurrentEnergy > 0.0f)
	{
		// Consuma il residuo e spegne
		UE_LOG(LogTemp, Warning, TEXT("Energia residua insufficiente. Consumo %.2f e spegnimento."), CurrentEnergy);
		CurrentEnergy = 0.0f;
	}
	else
	{
		// Energia già esaurita
		UE_LOG(LogTemp, Warning, TEXT("Energia esaurita. Spegnimento automatico."));
	}

	if (CurrentEnergy <= 0.0f)
	{
		bIsPoweredOn = false;
		GetWorldTimerManager().ClearTimer(ConsumptionTimer);

		OnPowerLost();

	}

}


void AConsumerEnergyActor::OnSolarEnergyProduced()
{
	bBlockedByRecentProduction = true;
	FTimerHandle UnblockHandle;
	GetWorldTimerManager().SetTimer(UnblockHandle, [this]()
		{
			bBlockedByRecentProduction = false;
		}, 1.0f, false);
}

void AConsumerEnergyActor::ForceShutdown()
{
	bIsPoweredOn = false;
	GetWorldTimerManager().ClearTimer(ConsumptionTimer);
	UE_LOG(LogTemp, Warning, TEXT("Consumer spento da SolarPanel per energia esaurita."));

}

void AConsumerEnergyActor::OnPowerLost()
{

}
