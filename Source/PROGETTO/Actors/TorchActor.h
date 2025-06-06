#pragma once

#include "CoreMinimal.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "Components/PointLightComponent.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "TorchActor.generated.h"


UCLASS()
class PROGETTO_API ATorchActor : public ABaseItem
{
    GENERATED_BODY()

public:
    ATorchActor();
    bool bIsOn;

};

