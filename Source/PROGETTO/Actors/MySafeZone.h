#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Widgets/SafeZoneMessageWidget.h"
#include "MySafeZone.generated.h"

class UBoxComponent;
class UAudioComponent;

UCLASS()
class AMySafeZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMySafeZone();

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponent;

	// Dichiarazione di un tipo di widget per il Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<USafeZoneMessageWidget> SafeZoneMessageWidgetClass;

	UPROPERTY()
	USafeZoneMessageWidget* SafeZoneMessageWidgetInstance;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ShowSafeZoneMessage();

	void RemoveSafeZoneMessage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
