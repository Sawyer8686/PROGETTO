#include "CompositeItem.h"
#include "Kismet/GameplayStatics.h"
#include "PROGETTO/PROGETTOCharacter.h"
#include "PROGETTO/Components/InventoryComponent.h"

ACompositeItem::ACompositeItem()
{
    // Puoi cambiare queste impostazioni in Blueprint
    ItemName = TEXT("Composite Item");
    //Description = TEXT("Smontami per ottenere le parti!");
    Description = FText::FromString(TEXT("Deposita"));
    ItemType = EItemType::Composite;  // o Equippable, a tuo piacere
    Weight = 5.f;                 // peso complessivo iniziale
    bStackable = false;
}

