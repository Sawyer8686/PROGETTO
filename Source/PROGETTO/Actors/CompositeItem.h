#pragma once

#include "CoreMinimal.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "CompositeItem.generated.h"

class UInventoryComponent;
class APROGETTOCharacter;

UCLASS()
class PROGETTO_API ACompositeItem : public ABaseItem
{
    GENERATED_BODY()

public:
    ACompositeItem();

    /** Classi delle parti da recuperare quando smonto questo oggetto */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Composite")
    TArray<TSubclassOf<ABaseItem>> PartClasses;

};

