// TorchBatteryWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TorchBatteryWidget.generated.h"

class UProgresssBar;

class UProgressBar;

/**
 * Widget che mostra la percentuale di batteria della torcia.
 */
UCLASS()
class PROGETTO_API UTorchBatteryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Questo ProgressBar verrà *bindato* dal Designer (WBP_TorchBatteryWidget). */
    UPROPERTY(meta = (BindWidget))
    UProgressBar* BatteryProgressBar;

    /** Imposta la percentuale (valore da 0.0 a 1.0). */
    UFUNCTION(BlueprintCallable)
    void SetBatteryPercent(float Percent);
    
};
