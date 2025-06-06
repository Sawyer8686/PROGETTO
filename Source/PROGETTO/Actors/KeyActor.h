#pragma once

#include "CoreMinimal.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "GameFramework/Actor.h"
#include "KeyActor.generated.h"

UCLASS()
class AKeyActor : public ABaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKeyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Implementazione dell'interfaccia IMainInteractionInterface */
	virtual void MainInteract(AActor* Interactor) override;

	//APROGETTOCharacter* Player;

	/** Identificatore univoco di questa chiave (deve corrispondere a quello richiesto dalla porta) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FName KeyID;

	/** Indica se questa chiave è già stata raccolta */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Key")
	bool bCollected;
};
