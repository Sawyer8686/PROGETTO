#pragma once

#include "EquipmentTypes.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    Head        UMETA(DisplayName = "Head"),
    RightHand   UMETA(DisplayName = "Right Hand"),
    LeftHand    UMETA(DisplayName = "Left Hand"),
    Torso       UMETA(DisplayName = "Torso"),
    RightLeg    UMETA(DisplayName = "Right Leg"),
    LeftLeg     UMETA(DisplayName = "Left Leg")
};