#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Interfaces/ConsumeEnergy.h"
#include "PROGETTO/Interfaces/MainInteractionInterface.h"
#include "LightSource.h"
#include "LightSwitch.generated.h"

UCLASS()
class ALightSwitch : public AActor, public IConsumeEnergy 
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALightSwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	// Luce da controllare
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	ALightSource* ConnectedLight;

	// Toggle chiamato da interazione
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Power")
	void TogglePower();
	virtual void TogglePower_Implementation() override;

};
