#pragma once

#include "CoreMinimal.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "BatteryActor.generated.h"


UCLASS()
class PROGETTO_API ABatteryActor : public ABaseItem
{
    GENERATED_BODY()

public:
    ABatteryActor();

    /** Quantità di carica che fornisce la batteria (se vorrai usarla in futuro) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battery")
    float ChargeAmount = 50.0f;
};
