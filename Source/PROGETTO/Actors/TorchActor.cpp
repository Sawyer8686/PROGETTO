#include "TorchActor.h"
#include "Components/PointLightComponent.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "Components/StaticMeshComponent.h"

ATorchActor::ATorchActor()
{
   
    ItemType = EItemType::Equippable;
    bIsEquipped = false;
    bIsOn = false;

    // La torcia può andare solo in mano destra o mano sinistra
    AllowedEquipmentSlots = {
        EEquipmentSlot::RightHand,
        EEquipmentSlot::LeftHand
    };
    
    
 
}







