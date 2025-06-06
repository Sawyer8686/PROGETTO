#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Interfaces/MainInteractionInterface.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "BaseItem.generated.h"

class UButton;

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Consumable,
    Food,
    Beverage,
    Equippable,
	Key,
	Tool,
	Weapon,
	Misc
};

UCLASS()
class ABaseItem : public AActor, public IMainInteractionInterface
{
	GENERATED_BODY()

public:
	
	ABaseItem();

public:
	
	virtual void BeginPlay() override;

    static void SetButtonDisabledBlack(UButton* Button);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

    /** Array di slot in cui questo item può essere equipaggiato */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment")
    TArray<EEquipmentSlot> AllowedEquipmentSlots;

	virtual void Tick(float DeltaTime) override;

	virtual void MainInteract(AActor* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemName;

    /** Descrizione */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText Description;

    /** Icona dell'oggetto */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* Icon;

    /** Tipo di oggetto */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType;

    /** Peso dell'oggetto */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float Weight;

    /** Se è impilabile (es. cibo, proiettili) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bStackable;

    /** Quantità corrente (usato se impilabile) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity;

    /** Quantità massima se impilabile */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    int32 MaxStackSize;

    /** Se può essere consumato (cibo, acqua, ecc.) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bCanBeConsumed;

    /** Se l’item è attualmente equipaggiato (solo valido se ItemType == Equippable) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    bool bIsEquipped;

    /** Quanta fame ripristina */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float HungerRestore;

    /** Quanta sete ripristina */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float ThirstRestore;
};
