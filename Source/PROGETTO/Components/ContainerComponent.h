#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ContainerComponent.generated.h"

class UChestWidget;
class APROGETTOCharacter;
class ABaseItem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROGETTO_API UContainerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UContainerComponent();

    /** Numero di slot disponibili */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
    int32 InventorySize = 10;

    /** Array degli oggetti correntemente nel baule */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Container")
    TArray<ABaseItem*> Inventory;

    /** Class del widget di interfaccia del baule */
    UPROPERTY(EditAnywhere, Category = "Container|UI")
    TSubclassOf<UChestWidget> ChestWidgetClass;

    /** Istanza correntemente aperta del widget */
    UPROPERTY()
    UChestWidget* ChestWidgetInstance = nullptr;

    /** Apre/chiude il widget del baule */
    void ToggleContainer(APROGETTOCharacter* ForCharacter);

    /** Aggiunge un item al baule, ritorna false se pieno/pesante */
    bool AddItemToContainer(ABaseItem* Item);

    UFUNCTION(BlueprintCallable, Category = "Container")
    bool HasSpaceForParts(const TArray<TSubclassOf<ABaseItem>>& PartClasses) const;

    /** Rimuove un item dal baule */
    bool RemoveItemFromContainer(ABaseItem* Item);

    /** Verifica spazio e peso residuo */
    bool HasSpaceForItem(ABaseItem* Item) const;

private:
    /** Riferimento al character che sta interagendo */
    APROGETTOCharacter* CachedCharacter = nullptr;
};

