#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "DayNightCycleComponent.generated.h"

// Definizione dei Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStart);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UDayNightCycleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDayNightCycleComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Set up the cycle duration in seconds (for example, 24 minutes for a full day-night cycle)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNightCycle")
	float DayLengthInSeconds = 10000.f; 

	// The Directional Light that simulates the Sun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNightCycle")
	AActor* SunActor;

	// Delegate per il cambio tra giorno e notte
	UPROPERTY(BlueprintAssignable, Category = "DayNightCycle")
	FOnDayStart OnDayStart;

	UPROPERTY(BlueprintAssignable, Category = "DayNightCycle")
	FOnNightStart OnNightStart;

	float CurrentTimeOfDay = 0.f;  // Time in the day (0-1 range)

	bool bIsDay = true; // Variabile che tiene traccia se è giorno o notte

	UFUNCTION(BlueprintCallable, Category = "DayNight")
	bool IsCurrentlyDay() const { return bIsDay; }

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:


	float TimeSpeed = 10.f;          // Speed modifier for the day-night cycle

	//bool bIsDay = true; // Variabile che tiene traccia se è giorno o notte

	

	void UpdateSunPosition();

	void CheckDayNightTransition();


};
