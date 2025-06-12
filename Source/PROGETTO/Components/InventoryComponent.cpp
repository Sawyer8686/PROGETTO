#include "InventoryComponent.h"
#include "PROGETTO/PROGETTOCharacter.h"                // Per accedere al Character proprietario
#include "PROGETTO/Actors/BackpackActor.h"
#include "PROGETTO/Widgets/InventoryWidget.h"
#include "PROGETTO/Widgets/StatsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PROGETTO/Actors/CompositeItem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    PrimaryComponentTick.bCanEverTick = false;
    // Valori di default dell’inventario
    InventorySize = 5;
    bHasBackpack = false;
    MaxCarryWeight = 20.f;
    CurrentCarryWeight = 0.f;
    InventoryWidgetInstance = nullptr;
    BackpackClass = nullptr;
    BackpackPickupSound = nullptr;
    InventoryOpenSound = nullptr;
    InventoryCloseSound = nullptr;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::MyToggleInventory()
{
    if (!bHasBackpack || !InventoryWidgetClass)
        return;

    APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner());
    if (!OwnerChar)
        return;
    APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
    if (!PC)
        return;

    // Crea il widget inventario la prima volta che viene aperto
    if (!InventoryWidgetInstance)
    {
        InventoryWidgetInstance = CreateWidget<UInventoryWidget>(OwnerChar->GetWorld(), InventoryWidgetClass);
        if (InventoryWidgetInstance)
        {
            InventoryWidgetInstance->OwningCharacter = OwnerChar;
            InventoryWidgetInstance->AddToViewport(100);
            InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
            if (InventoryOpenSound)
            {
                UGameplayStatics::PlaySound2D(this, InventoryOpenSound);
            }
        }
    }
    if (!InventoryWidgetInstance)
        return;

    if (InventoryWidgetInstance->IsVisible())
    {
        // **Chiusura** dell’inventario
        InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        if (InventoryCloseSound)
        {
            UGameplayStatics::PlaySound2D(this, InventoryCloseSound);
        }
        // Riabilita il movimento del personaggio e nasconde il cursore
        OwnerChar->EnableInput(PC);
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
        // Riespone il widget delle statistiche se esiste
        if (OwnerChar->StatsWidgetInstance)
        {
            OwnerChar->StatsWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        if (!InventoryWidgetInstance->IsInViewport())
             {
            InventoryWidgetInstance->AddToViewport(100);
            }

        // **Apertura** dell’inventario
        InventoryWidgetInstance->UpdateEquippedDisplay();
        InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        if (InventoryOpenSound)
        {
            UGameplayStatics::PlaySound2D(this, InventoryOpenSound);
        }
        // Aggiorna il widget con gli oggetti correnti e peso
        InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
        // Disabilita il movimento del personaggio e mostra il cursore per interagire con la UI
        OwnerChar->DisableInput(PC);
        PC->SetShowMouseCursor(true);
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        // Imposta il focus sul primo bottone dell’inventario
        InventoryWidgetInstance->FocusFirstButton();
        // Nasconde il widget delle statistiche mentre l’inventario è aperto
        if (OwnerChar->StatsWidgetInstance)
        {
            OwnerChar->StatsWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

bool UInventoryComponent::AddItemToInventory(ABaseItem* Item)
{
    if (!bHasBackpack || !Item)
        return false;

    APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner());
    if (!OwnerChar || !bHasBackpack || !Item)
        return false;

    float NewWeight = CurrentCarryWeight + Item->Weight;
    if (NewWeight > MaxCarryWeight)
    {
        // Peso totale eccede la capacità
        if (APROGETTOCharacter* Char = Cast<APROGETTOCharacter>(GetOwner()))
        {
            Char->ShowNotification(TEXT("Zaino troppo pieno! Non puoi raccogliere questo oggetto."), 2.0f);
        }
        return false;
    }

    // Pulisce eventuali elementi nulli dall’array
    Inventory.Remove(nullptr);
    int32 ValidCount = 0;
    for (ABaseItem* I : Inventory)
    {
        if (I) ValidCount++;
    }
    if (ValidCount >= InventorySize)
    {
        // Inventario già pieno
        if (APROGETTOCharacter* Char = Cast<APROGETTOCharacter>(GetOwner()))
        {
            Char->ShowNotification(TEXT("Inventario pieno!"), 2.0f);
        }
        return false;
    }

    if (Item->ItemType == EItemType::Equippable)
    {
        // Oggetto equipaggiabile (es. torcia)
        Inventory.Add(Item);
        CurrentCarryWeight = NewWeight;
        if (APROGETTOCharacter* Char = Cast<APROGETTOCharacter>(GetOwner()))
        {
            Char->ShowNotification(
                FString::Printf(TEXT("Hai raccolto: %s"), *Item->ItemName), 1.5f);
        }
        // Nasconde l’attore raccolto nel mondo (non distruggerlo)
        Item->SetActorHiddenInGame(true);
        Item->SetActorEnableCollision(false);
        if (UStaticMeshComponent* MeshComp = Item->FindComponentByClass<UStaticMeshComponent>())
        {
            MeshComp->SetVisibility(false, true);
            MeshComp->SetSimulatePhysics(false);
            MeshComp->SetEnableGravity(false);
        }
        return true;
    }

    // Oggetto generico
    Inventory.Add(Item);
    CurrentCarryWeight = NewWeight;
    if (APROGETTOCharacter* MyChar = Cast<APROGETTOCharacter>(GetOwner()))
    {
        MyChar->ShowNotification(
            FString::Printf(TEXT("Raccolto: %s"), *Item->GetName()), 1.5f);
    }
    Item->SetActorHiddenInGame(true);
    Item->SetActorEnableCollision(false);
    // Aggiorna la UI se l'inventario è aperto
    if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
    {
        InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
        InventoryWidgetInstance->UpdateEquippedDisplay();
    }
    // Disabilita la visibilità e fisica sull’oggetto raccolto
    if (UStaticMeshComponent* MeshComp = Item->FindComponentByClass<UStaticMeshComponent>())
    {
        MeshComp->SetVisibility(false, true);
        MeshComp->SetSimulatePhysics(false);
        MeshComp->SetEnableGravity(false);
    }
    return true;
}

bool UInventoryComponent::RemoveItemFromInventory(ABaseItem* Item)
{
    if (!Item)
        return false;
    if (Inventory.Remove(Item) > 0)
    {
        // Rimuove eventuali spazi vuoti rimasti nell’array
        Inventory.Remove(nullptr);
        CurrentCarryWeight -= Item->Weight;
        if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
        {
            InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
        }
        return true;
    }
    return false;
}

void UInventoryComponent::DropItem(ABaseItem* Item)
{


    if (!Item) return;
    APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner());
    if (!OwnerChar) return;
    UWorld* World = OwnerChar->GetWorld();
    if (!World) return;

    // Per altri oggetti, li spawniamo nuovamente come ABaseItem
    FVector CamLocation2 = OwnerChar->FollowCamera->GetComponentLocation();
    FVector CamForward2 = OwnerChar->FollowCamera->GetForwardVector().GetSafeNormal();
    const float DropDistance2 = 100.0f;
    FVector NominalPos2 = CamLocation2 + CamForward2 * DropDistance2;
    // Traccia un raggio per trovare terreno
    FVector TraceStart2 = NominalPos2 + FVector(0, 0, 500.0f);
    FVector TraceEnd2 = NominalPos2 - FVector(0, 0, 500.0f);
    FHitResult Hit2;
    FCollisionQueryParams Params2;
    Params2.AddIgnoredActor(OwnerChar);
    bool bHit2 = World->LineTraceSingleByChannel(Hit2, TraceStart2, TraceEnd2, ECC_Visibility, Params2);
    FVector FinalPos2;
    if (bHit2)
    {
        FinalPos2 = Hit2.ImpactPoint + FVector(0, 0, 10.0f);
        DrawDebugSphere(World, FinalPos2, 25.0f, 12, FColor::Blue, false, 3.0f);
    }
    else
    {
        FinalPos2 = NominalPos2 + FVector(0, 0, 100.0f);
        DrawDebugSphere(World, FinalPos2, 25.0f, 12, FColor::Red, false, 3.0f);
    }
    // Crea un nuovo attore ABaseItem identico a quello droppato
    FRotator FinalRot2 = CamForward2.Rotation();
    UClass* ItemClass = Item->GetClass();
    FActorSpawnParameters SpawnParams2;
    SpawnParams2.Owner = OwnerChar;
    SpawnParams2.Instigator = OwnerChar->GetInstigator();
    ABaseItem* NewDroppedItem = World->SpawnActor<ABaseItem>(ItemClass, FinalPos2, FinalRot2, SpawnParams2);
    if (NewDroppedItem)
    {
        NewDroppedItem->ItemName = Item->ItemName;
        NewDroppedItem->Description = Item->Description;
        NewDroppedItem->Icon = Item->Icon;
        NewDroppedItem->ItemType = Item->ItemType;
        NewDroppedItem->Weight = Item->Weight;
        NewDroppedItem->bStackable = Item->bStackable;
        NewDroppedItem->Quantity = Item->Quantity;
        NewDroppedItem->MaxStackSize = Item->MaxStackSize;
        NewDroppedItem->bCanBeConsumed = Item->bCanBeConsumed;
        NewDroppedItem->HungerRestore = Item->HungerRestore;
        NewDroppedItem->ThirstRestore = Item->ThirstRestore;
        if (UStaticMeshComponent* MeshComp = NewDroppedItem->FindComponentByClass<UStaticMeshComponent>())
        {
            MeshComp->SetSimulatePhysics(true);
            MeshComp->SetEnableGravity(true);
            MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
            MeshComp->WakeAllRigidBodies();
            FVector Impulse = CamForward2 * 200.0f;
            MeshComp->AddImpulse(Impulse, NAME_None, true);
        }
    }
    // Rimuove l’oggetto dall’inventario del personaggio
    RemoveItemFromInventory(Item);
}

void UInventoryComponent::EquipItem(ABaseItem* Item)
{
    if (!Item || Item->ItemType != EItemType::Equippable)
        return;
    if (!Item->bIsEquipped)
    {
        Item->bIsEquipped = true;
        if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
        {
            OwnerChar->ShowNotification(
                FString::Printf(TEXT("Equipped: %s"), *Item->ItemName), 1.0f);
        }
    }
}

bool UInventoryComponent::EquipItemToSlot(ABaseItem* Item, EEquipmentSlot Slot)
{
    if (!Item || Item->ItemType != EItemType::Equippable)
    {
        if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
        {
            OwnerChar->ShowNotification(TEXT("Item non è equipaggiabile"), 1.5f);
        }
        return false;
    }
    if (!Item->AllowedEquipmentSlots.Contains(Slot))
    {
        if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
        {
            OwnerChar->ShowNotification(TEXT("Questo oggetto non può essere equipaggiato qui"), 1.5f);
        }
        return false;
    }
    // Controlla se lo slot è già occupato
    if (ABaseItem** AlreadyEquipped = EquippedItemSlots.Find(Slot))
    {
        if (*AlreadyEquipped)
        {
            if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
            {
                OwnerChar->ShowNotification(TEXT("Slot già occupato"), 1.5f);
            }
            return false;
        }
    }
    // Se l’item era già equipaggiato in un altro slot, rimuovilo prima
    for (auto It = EquippedItemSlots.CreateIterator(); It; ++It)
    {
        if (It->Value == Item)
        {
            It.RemoveCurrent();
            Item->bIsEquipped = false;
            break;
        }
    }
    // Rimuove l’oggetto dall’inventario se presente
    int32 IndexInInventory = Inventory.Find(Item);
    if (IndexInInventory != INDEX_NONE)
    {
        Inventory.RemoveAtSwap(IndexInInventory);
    }
    // Marca l’oggetto come equipaggiato
    EquipItem(Item);
    // Aggiunge nella mappa dello slot
    EquippedItemSlots.Add(Slot, Item);
    if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
    {
        OwnerChar->ShowNotification(
            FString::Printf(TEXT("Equipped %s in %s"),
                *Item->ItemName, *UEnum::GetValueAsString(Slot)), 1.5f);
    }
    // Aggiorna la UI se aperta
    if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
    {
        InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
        InventoryWidgetInstance->UpdateEquippedDisplay();
    }
    return true;
}

void UInventoryComponent::UnequipItemFromSlot(EEquipmentSlot Slot)
{
    ABaseItem** FoundPtr = EquippedItemSlots.Find(Slot);
    if (!FoundPtr || !(*FoundPtr))
    {
        if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
        {
            OwnerChar->ShowNotification(TEXT("Nessun item in questo slot"), 1.0f);
        }
        return;
    }
    // Rimuove l’oggetto dallo slot e lo rimette nell’inventario
    ABaseItem* Item = *FoundPtr;
    EquippedItemSlots.Remove(Slot);
    Item->bIsEquipped = false;
    Inventory.Add(Item);
    if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
    {
        OwnerChar->ShowNotification(
            FString::Printf(TEXT("Unequipped: %s"), *Item->ItemName), 1.0f);
    }
    if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
    {
        InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
        InventoryWidgetInstance->UpdateEquippedDisplay();
    }
}

void UInventoryComponent::UnequipItem(ABaseItem* Item)
{
    if (!Item || Item->ItemType != EItemType::Equippable)
        return;
    // Trova in quale slot l’oggetto è equipaggiato
    EEquipmentSlot SlotFound;
    bool bFound = false;
    for (auto& Pair : EquippedItemSlots)
    {
        if (Pair.Value == Item)
        {
            SlotFound = Pair.Key;
            bFound = true;
            break;
        }
    }
    if (bFound)
    {
        EquippedItemSlots.Remove(SlotFound);
        if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
        {
            InventoryWidgetInstance->UpdateEquippedDisplay();
        }
        Item->bIsEquipped = false;
        if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
        {
            OwnerChar->ShowNotification(
                FString::Printf(TEXT("Unequipped: %s"), *Item->ItemName), 1.0f);
        }
    }
}

void UInventoryComponent::GiveBackpack()
{
    // Riproduce il suono di raccolta zaino se presente
    if (BackpackPickupSound)
    {
        if (APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner()))
        {
            UGameplayStatics::PlaySoundAtLocation(OwnerChar, BackpackPickupSound, OwnerChar->GetActorLocation());
        }
    }
    bHasBackpack = true;
}

void UInventoryComponent::DropBackpack()
{
    APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner());
    if (!OwnerChar || !bHasBackpack || BackpackClass == nullptr)
        return;

    // 1) Calcola una posizione di drop davanti al personaggio
    FVector Start = OwnerChar->GetActorLocation();
    FVector Forward = OwnerChar->GetActorForwardVector();
    const float ForwardDist = 100.0f;
    FVector DropLocation = Start + Forward * ForwardDist;
    FRotator DropRotation = OwnerChar->GetActorRotation();
    // 2) Parametri di spawn
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerChar;
    SpawnParams.Instigator = OwnerChar->GetInstigator();
    // 3) Spawna l’attore zaino nel mondo
    if (ABackpackActor* DroppedBackpack = OwnerChar->GetWorld()->SpawnActor<ABackpackActor>(
        BackpackClass, DropLocation, DropRotation, SpawnParams))
    {
        // 4) Disabilita lo zaino nel personaggio (perde l'inventario)
        bHasBackpack = false;
        // 5) Abilita simulazione fisica sul mesh dello zaino spawnato
        if (DroppedBackpack->Mesh)
        {
            DroppedBackpack->Mesh->SetSimulatePhysics(true);
            DroppedBackpack->Mesh->SetEnableGravity(true);
            DroppedBackpack->Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
            DroppedBackpack->Mesh->WakeAllRigidBodies();
            // 6) Applica un impulso iniziale in avanti per far volare un po' lo zaino
            const float ImpulseStrength = 150.0f;
            FVector Impulse = Forward * ImpulseStrength;
            DroppedBackpack->Mesh->AddImpulse(Impulse, NAME_None, true);
        }
        // Chiude il widget di inventario se era aperto
        if (InventoryWidgetInstance && InventoryWidgetInstance->IsInViewport())
        {
            InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
            if (APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController()))
            {
                PC->SetShowMouseCursor(false);
                PC->SetInputMode(FInputModeGameOnly());
            }
        }
    }
}

TArray<ABaseItem*> UInventoryComponent::GetEquippedItems() const
{
    TArray<ABaseItem*> Result;
    for (ABaseItem* I : Inventory)
    {
        if (I && I->bIsEquipped)
        {
            Result.Add(I);
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("GetEquippedItems: trovato %d equipaggiati"), Result.Num());
    return Result;
}

void UInventoryComponent::AddKey(FName KeyID)
{
    CollectedKeys.Add(KeyID);
}

void UInventoryComponent::RemoveKey(FName KeyID)
{
    CollectedKeys.Remove(KeyID);
}

void UInventoryComponent::DismantleItem(ABaseItem* Item)
{
    if (!Item || Item->ItemType != EItemType::Composite)
        return;

    // Cast sicuro
    ACompositeItem* Composite = Cast<ACompositeItem>(Item);
    if (!Composite)
        return;

    // Controllo spazio prima di smontare
    if (!HasSpaceForParts(Composite->PartClasses))
    {
        if (APROGETTOCharacter* PC = Cast<APROGETTOCharacter>(GetOwner()))
        {
            PC->ShowNotification(TEXT("Inventario pieno o troppo pesante!"), 2.0f);
        }
        return;
    }

    // Smontaggio: come prima
    UWorld* World = GetWorld();
    APROGETTOCharacter* OwnerChar = Cast<APROGETTOCharacter>(GetOwner());
    if (!World || !OwnerChar)
        return;

    for (auto& PartClass : Composite->PartClasses)
    {
        if (!PartClass) continue;
        FActorSpawnParameters Params;
        Params.Owner = OwnerChar;
        Params.Instigator = OwnerChar->GetInstigator();

        ABaseItem* NewPart = World->SpawnActor<ABaseItem>(
            PartClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
        if (NewPart)
            AddItemToInventory(NewPart);
    }

    RemoveItemFromInventory(Composite);
    Composite->Destroy();
}

bool UInventoryComponent::HasSpaceForParts(const TArray<TSubclassOf<ABaseItem>>& PartClasses) const
{
    // 1) Conteggio slot liberi
    int32 ValidCount = 0;
    for (ABaseItem* I : Inventory) if (I) ++ValidCount;
    int32 FreeSlots = InventorySize - ValidCount;
    if (FreeSlots < PartClasses.Num())
    {
        return false;
    }

    // 2) Calcolo peso totale delle parti
    float TotalWeight = 0.f;
    for (auto& Cls : PartClasses)
    {
        if (Cls)
        {
            if (ABaseItem* Default = Cls->GetDefaultObject<ABaseItem>())
            {
                TotalWeight += Default->Weight;
            }
        }
    }
    if (CurrentCarryWeight + TotalWeight > MaxCarryWeight)
    {
        return false;
    }

    return true;
}
