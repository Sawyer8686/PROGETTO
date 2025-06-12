#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CompositeItem.h" // per ABaseItem
#include "PROGETTO/Interfaces/MainInteractionInterface.h"
#include "BauleActor.generated.h"

class UContainerComponent;
class APROGETTOCharacter;
class UStaticMeshComponent;

UCLASS()
class PROGETTO_API ABauleActor : public AActor, public IMainInteractionInterface
{
    GENERATED_BODY()

public:
    ABauleActor();

    /** Componente che funge da container */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Container")
    UContainerComponent* ContainerComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
    UStaticMeshComponent* MeshComponent;

    virtual void MainInteract(AActor* Interactor) override;


protected:
    virtual void BeginPlay() override;

};

