#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Interfaces/ConsumeEnergy.h"
#include "PROGETTO/Interfaces/MainInteractionInterface.h"
#include "ConsumerEnergyActor.generated.h"

UCLASS()
class AConsumerEnergyActor : public AActor, public IConsumeEnergy
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AConsumerEnergyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TogglePower_Implementation();

	UPROPERTY(EditAnywhere, Category = "Energy")
	float EnergyConsumptionPerCycle = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Energy")
	float EnergyConsumptionInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Energy")
	class ASolarPanelActor* ConnectedSolarPanel;

	void ConsumeEnergy();

	UFUNCTION()
	void OnSolarEnergyProduced();

	FTimerHandle ConsumptionTimer;
	bool bIsPoweredOn = false;
	bool bBlockedByRecentProduction = false;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent * MeshComponent;

	bool IsPoweredOn() const { return bIsPoweredOn; }
	void ForceShutdown();

	virtual void OnPowerLost(); // Override dalle classi figlie
	
	
};
