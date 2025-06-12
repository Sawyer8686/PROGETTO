#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "InventoryComponent.generated.h"

class UInventoryWidget;
class ABackpackActor;
class USoundBase;
class APROGETTOCharacter;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

    /** Dimensione massima dello zaino (numero di slot) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 InventorySize;

    /** Array degli oggetti attualmente nell段nventario */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<ABaseItem*> Inventory;

    /** Slot di equipaggiamento correnti (mappa Slot -> Item equipaggiato) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TMap<EEquipmentSlot, ABaseItem*> EquippedItemSlots;

    /** Peso massimo trasportabile */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    float MaxCarryWeight;

    /** Peso attuale trasportato */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    float CurrentCarryWeight;

    /** True se il personaggio possiede lo zaino (inventario abilitato) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    bool bHasBackpack;

    /** Classe del widget di inventario da istanziare */
    UPROPERTY(EditAnywhere, Category = "Inventory UI")
    TSubclassOf<UInventoryWidget> InventoryWidgetClass;

    /** Istanza corrente del widget di inventario (se aperto) */
    UPROPERTY()
    UInventoryWidget* InventoryWidgetInstance;

    /** Classe dell誕ttore zaino da spawnare quando il personaggio lo lascia cadere */
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<ABackpackActor> BackpackClass;

    /** Suono riproducibile alla raccolta dello zaino */
    UPROPERTY(EditAnywhere, Category = "Inventory|Sound")
    USoundBase* BackpackPickupSound;
    /** Suono di apertura inventario */
    UPROPERTY(EditAnywhere, Category = "Inventory|Sound")
    USoundBase* InventoryOpenSound;
    /** Suono di chiusura inventario */
    UPROPERTY(EditAnywhere, Category = "Inventory|Sound")
    USoundBase* InventoryCloseSound;

    /** Apertura/chiusura dell段nventario (UI) */
    void MyToggleInventory();
    /** Aggiunge un oggetto all段nventario */
    bool AddItemToInventory(ABaseItem* Item);
    /** Rimuove un oggetto dall段nventario */
    bool RemoveItemFromInventory(ABaseItem* Item);
    /** Getta a terra un oggetto dall段nventario */
    void DropItem(ABaseItem* Item);
    /** Segna un oggetto come equipaggiato (aggiorna stato) */
    void EquipItem(ABaseItem* Item);
    /** Equipaggia un oggetto in uno specifico slot */
    bool EquipItemToSlot(ABaseItem* Item, EEquipmentSlot Slot);
    /** Sfila (unequip) un oggetto equipaggiato */
    void UnequipItem(ABaseItem* Item);
    /** Sfila (unequip) l弛ggetto da uno slot specifico */
    void UnequipItemFromSlot(EEquipmentSlot Slot);
    /** Abilita l段nventario (quando il personaggio raccoglie lo zaino) */
    void GiveBackpack();
    /** Fa cadere lo zaino a terra (disabilita l段nventario) */
    void DropBackpack();
    /** Restituisce gli oggetti attualmente equipaggiati */
    TArray<ABaseItem*> GetEquippedItems() const;
    /** Aggiunge un identificatore di chiave alla lista delle chiavi raccolte */
    void AddKey(FName KeyID);
    /** Rimuove un identificatore di chiave dalla lista delle chiavi raccolte */
    void RemoveKey(FName KeyID);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void DismantleItem(ABaseItem * Item);

    /** Verifica se ci sono slot e peso liberi per queste parti */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasSpaceForParts(const TArray<TSubclassOf<ABaseItem>>& PartClasses) const;

    /** Insieme delle chiavi raccolte (identificatori) */
    UPROPERTY()
    TSet<FName> CollectedKeys;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
