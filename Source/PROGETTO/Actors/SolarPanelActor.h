#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Widgets/EnergyWidget.h"
#include "Sound/SoundBase.h"
#include "PROGETTO/Interfaces/MainInteractionInterface.h"
#include "Blueprint/UserWidget.h"
#include "SolarPanelActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnergyProduced);

UCLASS()
class ASolarPanelActor : public AActor, public IMainInteractionInterface
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	ASolarPanelActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Solar Panel")
	bool bIsEnabled;

	UFUNCTION(BlueprintCallable, Category = "Solar Panel")
	void ToggleSolarPanel();

	virtual void MainInteract(AActor* Interactor) override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMeshComponent*> PanelMeshes;

	void SetMaterialOnAllPanels(UMaterialInterface* Material);

	UPROPERTY(EditAnywhere, Category = "Visual")
	UMaterialInterface* OnMaterial;

	UPROPERTY(EditAnywhere, Category = "Visual")
	UMaterialInterface* OffMaterial;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Timer per la produzione di energia
	FTimerHandle EnergyTimerHandle;

	// Funzione chiamata dal timer
	void ProduceEnergy();

	// Funzioni collegate ai delegate
	UFUNCTION()
	void HandleDayStart();

	UFUNCTION()
	void HandleNightStart();

	// Produce energia ogni X secondi durante il giorno
	UPROPERTY(EditAnywhere, Category = "Solar Panel")
	float EnergyProductionInterval;

	// Evento broadcast per notifica esterna (opzionale)
	UPROPERTY(BlueprintAssignable, Category = "Solar Panel")
	FOnEnergyProduced OnEnergyProduced;

	// Riferimento al gestore del ciclo giorno/notte
	UPROPERTY(BlueprintReadWrite, Category = "Solar Panel")
	UActorComponent* DayNightManager;

	void RegisterToDayNightCycle();

	// Energia attuale accumulata
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Solar Panel")
	float CurrentEnergy = 0.0f;

	// Energia generata ogni ciclo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Solar Panel")
	float EnergyPerCycle;

	// Energia massima accumulabile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Solar Panel")
	float MaxEnergy = 100.0f;

	UFUNCTION(BlueprintCallable, Category = "Solar Panel")
	float GetCurrentEnergy() const { return CurrentEnergy; }

	UFUNCTION(BlueprintCallable, Category = "Solar Panel")
	float GetMaxEnergy() const { return MaxEnergy; }

	bool bIsDay ;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EnergyWidgetClass;

	UEnergyWidget* EnergyWidgetInstance;

	UPROPERTY(BlueprintReadOnly)
	bool bIsProducingEnergy = false;

};
