#include "SolarPanelActor.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "PROGETTO/PROGETTOGameMode.h"
#include "PROGETTO/Components/DayNightCycleComponent.h"
#include "Kismet/GameplayStatics.h"

ASolarPanelActor::ASolarPanelActor()
{
	
	PrimaryActorTick.bCanEverTick = true;

	//PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));
	//RootComponent = PanelMesh;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	// Esempio: creiamo 3 pannelli
	for (int32 i = 0; i < 3; ++i)
	{
		FString Name = FString::Printf(TEXT("PanelMesh_%d"), i);
		UStaticMeshComponent* NewPanel = CreateDefaultSubobject<UStaticMeshComponent>(*Name);
		NewPanel->SetupAttachment(RootComponent);
		NewPanel->SetRelativeLocation(FVector(i * 100.f, 0.f, 0.f)); // posizionamento semplice

		PanelMeshes.Add(NewPanel);
	}

	CurrentEnergy = 0.0f;
	MaxEnergy = 100.0f;
	EnergyPerCycle = 0.025f;
	EnergyProductionInterval = 0.02f;

}

void ASolarPanelActor::MainInteract(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("MainInteract chiamato su SolarPanel!"));
	ToggleSolarPanel();

}

void ASolarPanelActor::SetMaterialOnAllPanels(UMaterialInterface* Material)
{
	for (UStaticMeshComponent* Panel : PanelMeshes)
	{
		if (Panel && Material)
		{
			Panel->SetMaterial(0, Material);
		}
	}

}

void ASolarPanelActor::BeginPlay()
{
	Super::BeginPlay();

	if (bIsEnabled && OnMaterial)
	{
		SetMaterialOnAllPanels(OnMaterial);
	}
	else if (OffMaterial)
	{
		SetMaterialOnAllPanels(OffMaterial);
	}

	// Ritarda la registrazione per garantire che il GameMode sia inizializzato
	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(DelayHandle, this, &ASolarPanelActor::RegisterToDayNightCycle, 0.2f, false);

	// Crea e mostra il widget dell’energia
	if (EnergyWidgetClass)
	{
		EnergyWidgetInstance = CreateWidget<UEnergyWidget>(GetWorld(), EnergyWidgetClass);
		if (EnergyWidgetInstance)
		{
			EnergyWidgetInstance->AddToViewport(); // Aggiunge il widget allo schermo
		}
	}
}

void ASolarPanelActor::ToggleSolarPanel()
{
	bIsEnabled = !bIsEnabled;

	UE_LOG(LogTemp, Warning, TEXT("ToggleSolarPanel: bIsEnabled = %s"), bIsEnabled ? TEXT("true") : TEXT("false"));

	if (bIsEnabled)
	{
		if (bIsDay)
		{
			UE_LOG(LogTemp, Warning, TEXT("Produzione energia AVVIATA (pannello acceso, è giorno)"));
			bIsProducingEnergy = true;
			GetWorldTimerManager().SetTimer(EnergyTimerHandle, this, &ASolarPanelActor::ProduceEnergy, EnergyProductionInterval, true);
		}

		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pannello acceso ma è notte, produzione NON avviata"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Produzione energia FERMA (pannello spento o è notte)"));
		bIsProducingEnergy = false;
		GetWorldTimerManager().ClearTimer(EnergyTimerHandle);
		NotifyConsumersPowerLost();
	}

	if (bIsEnabled && bIsDay && OnMaterial)
	{
		SetMaterialOnAllPanels(OnMaterial);
	}
	else if (OffMaterial)
	{
		SetMaterialOnAllPanels(OffMaterial);
	}
}

void ASolarPanelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnergyWidgetInstance)
	{
		float Current = GetCurrentEnergy();
		float Max = GetMaxEnergy();
		EnergyWidgetInstance->SetEnergyLevel(Current / Max);
	}

}

void ASolarPanelActor::ProduceEnergy()
{
	
	if (CurrentEnergy >= MaxEnergy)
	{
		bIsProducingEnergy = false;
		GetWorldTimerManager().ClearTimer(EnergyTimerHandle);
		return;
	}

	else

	{
		bIsProducingEnergy = true;

		const float EnergyToAdd = FMath::Min(EnergyPerCycle, MaxEnergy - CurrentEnergy);
		CurrentEnergy += EnergyToAdd;

		OnEnergyProduced.Broadcast();
	}

	

}

void ASolarPanelActor::HandleDayStart()
{
	bIsDay = true;

	if (bIsEnabled)
	{
		bIsProducingEnergy = true;
		GetWorldTimerManager().SetTimer(EnergyTimerHandle, this, &ASolarPanelActor::ProduceEnergy, EnergyProductionInterval, true);

	}

	if (bIsEnabled && OnMaterial)
	{
		SetMaterialOnAllPanels(OnMaterial);
	}
	
}

void ASolarPanelActor::HandleNightStart()
{
	bIsDay = false;

	

	bIsProducingEnergy = false;
	GetWorldTimerManager().ClearTimer(EnergyTimerHandle);
	//NotifyConsumersPowerLost();

	if (OffMaterial)
	{
		SetMaterialOnAllPanels(OffMaterial);
	}
	

}

void ASolarPanelActor::RegisterToDayNightCycle()
{

	if (APROGETTOGameMode* MyGameMode = Cast<APROGETTOGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (UDayNightCycleComponent* DayNightComponent = MyGameMode->GetDayNightCycleComponent())
		{
			DayNightComponent->OnDayStart.AddDynamic(this, &ASolarPanelActor::HandleDayStart);
			DayNightComponent->OnNightStart.AddDynamic(this, &ASolarPanelActor::HandleNightStart);

			// Controlla se è già giorno
			if (DayNightComponent->IsCurrentlyDay())
			{

				HandleDayStart();
			}
		}
	}
}

void ASolarPanelActor::RegisterConsumer(AConsumerEnergyActor* Consumer)
{
	if (Consumer && !ConnectedConsumers.Contains(Consumer))
	{
		ConnectedConsumers.Add(Consumer);
	}
}

void ASolarPanelActor::NotifyConsumersPowerLost()
{
	for (AConsumerEnergyActor* Consumer : ConnectedConsumers)
	{
		if (Consumer && Consumer->IsPoweredOn())
		{
			Consumer->ForceShutdown();
		}
	}
}


