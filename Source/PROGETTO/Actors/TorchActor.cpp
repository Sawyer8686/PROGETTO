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

void ATorchActor::MainInteract(AActor* Interactor)
{
    if (APROGETTOCharacter* Player = Cast<APROGETTOCharacter>(Interactor))

    {
        bool bTorchAdded = Player->AddItemToInventory(this);

        if (bTorchAdded)
        {
            Player->bHasTorchEquipped = true;
            Player->bHasTorch = true;
            SetActorHiddenInGame(true);
            SetActorEnableCollision(false);
            UE_LOG(LogTemp, Warning, TEXT("TORCIA RACCOLTA"));
        }

    }
}







