#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Interfaces/MainInteractionInterface.h"
#include "BackpackActor.generated.h"

UCLASS()
class ABackpackActor : public AActor, public IMainInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABackpackActor();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Backpack")
	UStaticMeshComponent* Mesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void MainInteract(AActor* Interactor) override;
};
