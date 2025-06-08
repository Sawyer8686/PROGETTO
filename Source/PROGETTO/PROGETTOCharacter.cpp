// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROGETTOCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "PROGETTO/Actors/KeyActor.h"
#include "PROGETTO/Widgets/StatsWidget.h"
#include "PROGETTO/Interfaces/ConsumeEnergy.h"
#include "PROGETTO/Actors/BackpackActor.h"
#include "Components/ProgressBar.h"
#include "PROGETTO/Actors/TorchActor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "Components/SpotLightComponent.h"
#include "Interfaces/MainInteractionInterface.h"
#include "Components/PointLightComponent.h"
#include "PROGETTO/Widgets/TorchBatteryWidget.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


APROGETTOCharacter::APROGETTOCharacter()
{
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 	
	CameraBoom->bUsePawnControlRotation = true; 

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	CharacterTorchSpot = CreateDefaultSubobject<USpotLightComponent>(TEXT("CharacterTorchSpot"));
	CharacterTorchSpot->SetupAttachment(FollowCamera);
	CharacterTorchSpot->bUseInverseSquaredFalloff = false; 
	CharacterTorchSpot->SetAttenuationRadius(1000.f);      
	CharacterTorchSpot->SetInnerConeAngle(20.0f);          
	CharacterTorchSpot->SetOuterConeAngle(30.0f);         
	CharacterTorchSpot->SetVisibility(false);             

	bHasTorchEquipped = false;
	EquippedTorch = nullptr;

	MaxBatteryEnergy = 100.0f;
	CurrentBatteryEnergy = MaxBatteryEnergy;
	BatteryDrainInterval = 1.0f;  
	BatteryDrainAmount = 5.0f;  
	bTorchIsOn = false;
	TorchBatteryWidgetInstance = nullptr;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	MaxHunger = 100.0f;
	CurrentHunger = MaxHunger;

	MaxThirst = 100.0f;
	CurrentThirst = MaxThirst;

	bIsRunning = false;

	Stamina = 100.0f;
	MaxStamina = 100.0f;

	TimeSinceStoppedRunning = 0.0f;
	bCanRecoverStamina = true;

	bStaminaSottoSoglia = false;

	InventorySize = 5;
	bHasBackpack = false;
	
	MaxCarryWeight = 20.f;  
	CurrentCarryWeight = 0.f;

}

void APROGETTOCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	CurrentHunger = MaxHunger;

	CurrentThirst = MaxThirst;

	if (StatsWidgetClass)
	{
		StatsWidgetInstance = CreateWidget<UStatsWidget>(GetWorld(), StatsWidgetClass);
		if (StatsWidgetInstance)
		{
			StatsWidgetInstance->AddToViewport(0);
			UpdateHealthUI();
		}
	}

	StartHungerTimer();
	StartThirstTimer();

}

void APROGETTOCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRunning)
	{
		if (Stamina > 0.f)
		{
			Stamina -= DeltaTime * 10.0f;
		}
		else
		{
			Stamina = 0.f;
			StopRunning(); 
		}

		
		if (Stamina / MaxStamina < 0.5f)
		{
			bStaminaSottoSoglia = true;
		}
	}
	else
	{
		
		if (!bCanRecoverStamina)
		{
			TimeSinceStoppedRunning += DeltaTime;
			if (TimeSinceStoppedRunning >= 2.0f)
			{
				bCanRecoverStamina = true;
			}
		}

	
		if (bCanRecoverStamina && Stamina < MaxStamina)
		{
			Stamina += DeltaTime * 5.0f;

			
			if (Stamina / MaxStamina >= 0.5f)
			{
				bStaminaSottoSoglia = false;
			}
		}
	}


	Stamina = FMath::Clamp(Stamina, 0.f, MaxStamina);

	// Aggiorna il Widget
	if (StatsWidgetClass)
	{
		StatsWidgetInstance->SetStaminaPercent(Stamina / MaxStamina);


	}

	if (bTorchIsOn && CharacterTorchSpot)
	{
		FVector Velocity = GetVelocity();
		Velocity.Z = 0;
		if (!Velocity.IsNearlyZero())
		{
			FRotator DesiredRotation = Velocity.Rotation();
			CharacterTorchSpot->SetWorldRotation(DesiredRotation);
		}
	}

}




///////////////////    INVENTARIO    ///////////////////////


void APROGETTOCharacter::ToggleInventory()
{
	

	if (!bHasBackpack || !InventoryWidgetClass)
		return;

	if (!InventoryWidgetInstance )
	{
		InventoryWidgetInstance = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
		if (InventoryWidgetInstance)
		{
			InventoryWidgetInstance->OwningCharacter = this;
			InventoryWidgetInstance->AddToViewport(100);
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!InventoryWidgetInstance)
		return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return;

	
	if (InventoryWidgetInstance->IsVisible())
	{
		
		InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);

		EnableInput(PC);
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());

		
		if (StatsWidgetInstance)
			StatsWidgetInstance->SetVisibility(ESlateVisibility::Visible);	
	}
	else
	{
		
		//InventoryWidgetInstance->SetMyInventoryItems(
			//Inventory,
			//CurrentCarryWeight,
			//MaxCarryWeight
		//);


		InventoryWidgetInstance->UpdateEquippedDisplay();
		InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);

		InventoryWidgetInstance->SetMyInventoryItems(
			Inventory,
			CurrentCarryWeight,
			MaxCarryWeight);

		DisableInput(PC);
		PC->SetShowMouseCursor(true);

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);

		InventoryWidgetInstance->FocusFirstButton();

		
		if (StatsWidgetInstance)
			StatsWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}

}

bool APROGETTOCharacter::AddItemToInventory(ABaseItem* Item)
{

	if (!bHasBackpack || !Item)
		return false;

	float NewWeight = CurrentCarryWeight + Item->Weight;

	if (NewWeight > MaxCarryWeight)
	{
		ShowNotification(TEXT("Zaino troppo pieno! Non puoi raccogliere questo oggetto."), 2.0f);
		return false;
	}

	Inventory.Remove(nullptr);

	
	int32 ValidCount = 0;
	for (ABaseItem* I : Inventory)
	{
		if (I)
			ValidCount++;
	}

	if (ValidCount >= InventorySize)
	{
		ShowNotification(TEXT("Inventario pieno!"), 2.0f);
		
		return false;
	}

	
	if (AKeyActor* Key = Cast<AKeyActor>(Item))
	{
		Inventory.Add(Key);
		CurrentCarryWeight = NewWeight;

		AddKey(Key->KeyID);
		ShowNotification(FString::Printf(TEXT("Hai raccolto la chiave: %s"), *Key->KeyID.ToString()), 2.0f);

		Key->SetActorHiddenInGame(true);
		Key->SetActorEnableCollision(false);
		Key->Mesh->SetVisibility(false, true);

		
		return true;
	}

	if (ATorchActor* InTorch = Cast<ATorchActor>(Item))
	{
		Inventory.Add(InTorch);                      // la inserisco nell’array Inventory
		CurrentCarryWeight += InTorch->Weight;       // aggiorno peso

		// nascondo nel mondo, ma NON distruggo:
		InTorch->SetActorHiddenInGame(true);
		InTorch->SetActorEnableCollision(false);

		ShowNotification(FString::Printf(TEXT("Hai raccolto la torcia")), 1.5f);
		return true;
	}

	if (Item->ItemType == EItemType::Equippable)
	{
		Inventory.Add(Item);
		CurrentCarryWeight = NewWeight;
		ShowNotification(FString::Printf(TEXT("Hai raccolto: %s"), *Item->ItemName), 1.5f);

		// Nascondo l’attore torcia nel mondo (ma NON lo distruggo)
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

	Inventory.Add(Item);
	CurrentCarryWeight = NewWeight;
	ShowNotification(FString::Printf(TEXT("Raccolto: %s"), *Item->GetName()), 1.5f);

	Item->SetActorHiddenInGame(true);
	Item->SetActorEnableCollision(false);

	// --- UI refresh solo se visibile ---
	if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
	{
		InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
		InventoryWidgetInstance->UpdateEquippedDisplay();
	}

	if (UStaticMeshComponent* MeshComp = Item->FindComponentByClass<UStaticMeshComponent>())
	{
		MeshComp->SetVisibility(false, true);
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetEnableGravity(false);
	}
	return true;
}

bool APROGETTOCharacter::RemoveItemFromInventory(ABaseItem* Item)
{
	//if (!Item)
		//return;

	//UE_LOG(LogTemp, Warning, TEXT("RemoveItemFromInventory: prima Inventory.Num()=%d"), Inventory.Num());

	////Inventory.RemoveSingleSwap(Item);

	//UE_LOG(LogTemp, Warning, TEXT("RemoveItemFromInventory: dopo Inventory.Num()=%d"), Inventory.Num());

	//CurrentCarryWeight = FMath::Max(0.f, CurrentCarryWeight - Item->Weight);

	if (!Item) return false;
	if (Inventory.Remove(Item) > 0)
	{
		Inventory.Remove(nullptr);  // elimina eventuali vuoti
		CurrentCarryWeight -= Item->Weight;
		if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
		{
			InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
		}
		return true;
	}
	return false;

}

void APROGETTOCharacter::DropItem(ABaseItem* Item)
{

	if (!Item)
		return;

	// ──── Se è esattamente la torcia equipaggiata, prima faccio l'Unequip ────
	if (ATorchActor* TorchActorPtr = Cast<ATorchActor>(Item))
	{
		if (bHasTorchEquipped)
		{
			UE_LOG(LogTemp, Warning, TEXT(">>>> DropItem: l'item è la torcia equipaggiata, chiamo UnequipItem"));
			UnequipItem(Item);
			// bHasTorchEquipped diventa false e la luce viene spenta qui dentro
		}
	}

	if (AKeyActor* Key = Cast<AKeyActor>(Item))
	{
		RemoveKey(Key->KeyID);

		FVector CamLoc = FollowCamera->GetComponentLocation();
		FVector CamFor = FollowCamera->GetForwardVector().GetSafeNormal();
		constexpr float Dist = 100.0f;
		FVector NominalPos = CamLoc + CamFor * Dist;
		FVector TraceStart = NominalPos + FVector(0, 0, 500.0f);
		FVector TraceEnd = NominalPos - FVector(0, 0, 500.0f);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			Params
		);

		FVector FinalPos;
		if (bHit)
		{
			FinalPos = Hit.ImpactPoint + FVector(0, 0, 10.0f);
			DrawDebugSphere(GetWorld(), FinalPos, 25.0f, 12, FColor::Green, false, 3.0f);

		}
		else
		{
			FinalPos = NominalPos + FVector(0, 0, 100.0f);
			DrawDebugSphere(GetWorld(), FinalPos, 25.0f, 12, FColor::Red, false, 3.0f);

		}


		UClass* KeyClass = Key->GetClass();
		FRotator SpawnRot = CamFor.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		AKeyActor* NewKey = GetWorld()->SpawnActor<AKeyActor>(
			KeyClass,
			FinalPos,
			SpawnRot,
			SpawnParams
		);

		if (NewKey)
		{

			NewKey->KeyID = Key->KeyID;
			NewKey->ItemName = Key->ItemName;
			NewKey->Description = Key->Description;
			NewKey->Icon = Key->Icon;
			NewKey->Weight = Key->Weight;
			NewKey->bCollected = false;


			if (UStaticMeshComponent* MeshComp = NewKey->FindComponentByClass<UStaticMeshComponent>())
			{
				MeshComp->SetSimulatePhysics(true);
				MeshComp->SetEnableGravity(true);
				MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
				MeshComp->WakeAllRigidBodies();

				FVector Impulse = CamFor * 200.0f;
				MeshComp->AddImpulse(Impulse, NAME_None, true);

			}


		}

		Key->Destroy();
		RemoveItemFromInventory(Item);

		return;
	}


	UWorld* World = GetWorld();
	if (!World)
		return;


	FVector CamLocation2 = FollowCamera->GetComponentLocation();
	FVector CamForward2 = FollowCamera->GetForwardVector().GetSafeNormal();
	constexpr float DropDistance2 = 100.0f;
	FVector NominalPos2 = CamLocation2 + CamForward2 * DropDistance2;
	FVector TraceStart2 = NominalPos2 + FVector(0, 0, 500.0f);
	FVector TraceEnd2 = NominalPos2 - FVector(0, 0, 500.0f);
	FHitResult Hit2;
	FCollisionQueryParams Params2;
	Params2.AddIgnoredActor(this);

	bool bHit2 = GetWorld()->LineTraceSingleByChannel(Hit2, TraceStart2, TraceEnd2, ECC_Visibility, Params2);
	FVector FinalPos2;
	if (bHit2)
	{
		FinalPos2 = Hit2.ImpactPoint + FVector(0, 0, 10.0f);
		DrawDebugSphere(GetWorld(), FinalPos2, 25.0f, 12, FColor::Blue, false, 3.0f);
	}
	else
	{
		FinalPos2 = NominalPos2 + FVector(0, 0, 100.0f);
		DrawDebugSphere(GetWorld(), FinalPos2, 25.0f, 12, FColor::Red, false, 3.0f);
	}

	FRotator FinalRot2 = CamForward2.Rotation();
	UClass* ItemClass = Item->GetClass();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	ABaseItem* NewDroppedItem = World->SpawnActor<ABaseItem>(
		ItemClass, FinalPos2, FinalRot2, SpawnParams);

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

	RemoveItemFromInventory(Item);

}

void APROGETTOCharacter::EquipItem(ABaseItem* Item)
{
	if (!Item || Item->ItemType != EItemType::Equippable)
		return;

	if (!Item->bIsEquipped)
	{
		Item->bIsEquipped = true;
		ShowNotification(FString::Printf(TEXT("Equipped: %s"), *Item->ItemName), 1.0f);

		if (ATorchActor* ATorch = Cast<ATorchActor>(Item))
		{
			bHasTorchEquipped = true;

			if (ATorchActor* BTorch = Cast<ATorchActor>(Item))
			{
				bHasTorchEquipped = true;

				// 1) Se non ho ancora il widget, lo creo
				if (TorchBatteryWidgetClass && !TorchBatteryWidgetInstance)
				{
					TorchBatteryWidgetInstance = CreateWidget<UTorchBatteryWidget>(
						GetWorld(), TorchBatteryWidgetClass
					);
					if (TorchBatteryWidgetInstance)
					{
						TorchBatteryWidgetInstance->AddToViewport(900);
						// Inizializzo subito la percentuale in base al valore corrente:
						float Percent = (MaxBatteryEnergy > 0.0f)
							? (CurrentBatteryEnergy / MaxBatteryEnergy)
							: 0.0f;
						TorchBatteryWidgetInstance->SetBatteryPercent(Percent);
					}
				}
				else if (TorchBatteryWidgetInstance)
				{
					TorchBatteryWidgetInstance->SetVisibility(ESlateVisibility::Visible);
					float Percent = (MaxBatteryEnergy > 0.0f)
						? (CurrentBatteryEnergy / MaxBatteryEnergy)
						: 0.0f;
					TorchBatteryWidgetInstance->SetBatteryPercent(Percent);
				}
			}
		}
	}

}

void APROGETTOCharacter::UnequipItemFromSlot(EEquipmentSlot Slot)
{
	ABaseItem** FoundPtr = EquippedItemSlots.Find(Slot);
	if (!FoundPtr || !(*FoundPtr))
	{
		ShowNotification(TEXT("Nessun item in questo slot"), 1.0f);
		return;
	}

	ABaseItem* Item = *FoundPtr;
	EquippedItemSlots.Remove(Slot);


	Item->bIsEquipped = false;

	// 4) Se era una torcia, gestisco anche quel flag e la rimostro nel mondo
	if (ATorchActor* ATorch = Cast<ATorchActor>(Item))
	{
		// Se la torcia era accesa, spegnila
		if (bTorchIsOn)
			TurnOffTorch();

		bHasTorchEquipped = false;
		EquippedTorch = nullptr;

		// Rimostro la torcia nel mondo
		ATorch->SetActorHiddenInGame(false);
		ATorch->SetActorEnableCollision(true);
	}

	// Se è una torcia, rimuovo o nascondo il widget della batteria
	if (ATorchActor* ATorch = Cast<ATorchActor>(Item))
	{
		bHasTorchEquipped = false;

		if (TorchBatteryWidgetInstance)
		{
			TorchBatteryWidgetInstance->RemoveFromParent();
			TorchBatteryWidgetInstance = nullptr;
		}
	}

	Inventory.Add(Item);

	ShowNotification(FString::Printf(TEXT("Unequipped: %s"), *Item->ItemName), 1.0f);

	if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
	{
		
		InventoryWidgetInstance->SetMyInventoryItems(Inventory, CurrentCarryWeight, MaxCarryWeight);
		InventoryWidgetInstance->UpdateEquippedDisplay();
	}
}

void APROGETTOCharacter::UnequipItem(ABaseItem* Item)
{
	if (!Item || Item->ItemType != EItemType::Equippable)
		return;

	// Cerco in quale slot era equip
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
		ShowNotification(FString::Printf(TEXT("Unequipped: %s"), *Item->ItemName), 1.0f);
	}

}



//////////////////   ZAINO    ////////////////////////


void APROGETTOCharacter::GiveBackpack()
{
	bHasBackpack = true;
}

TArray<ABaseItem*> APROGETTOCharacter::GetEquippedItems() const
{
	
	TArray<ABaseItem*> Result;
	for (ABaseItem* I : Inventory)
	{
		if (I && I->bIsEquipped)
			Result.Add(I);
	}
	UE_LOG(LogTemp, Warning, TEXT("GetEquippedItems: trovato %d equipaggiati"), Result.Num());
	return Result;
}

void APROGETTOCharacter::DropBackpack()
{
	if (!bHasBackpack || !BackpackClass)
		return;

	// 1) Calcolo posizione di spawn qualche unità davanti al personaggio
	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	const float ForwardDist = 100.0f;
	FVector DropLocation = Start + Forward * ForwardDist;
	FRotator DropRotation = GetActorRotation();

	// 2) Parametri di spawn
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 3) Spawno lo zaino nel mondo
	DroppedBackpack = GetWorld()->SpawnActor<ABackpackActor>(
		BackpackClass,
		DropLocation,
		DropRotation,
		SpawnParams
	);

	if (DroppedBackpack)
	{
		// 4) Disabilito lo zaino nel personaggio
		bHasBackpack = false;

		// 6) Abilito simulazione fisica sul componente Mesh dello zaino spawnato
		if (DroppedBackpack->Mesh)
		{
			// Forzo simulazione fisica e gravità
			DroppedBackpack->Mesh->SetSimulatePhysics(true);
			DroppedBackpack->Mesh->SetEnableGravity(true);

			// Uso un profilo di collisione adatto per oggetti fisici
			DroppedBackpack->Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

			// Risveglio eventuali rigid bodies (per sicurezza)
			DroppedBackpack->Mesh->WakeAllRigidBodies();

			// 7) Applico un impulso iniziale in avanti
			const float ImpulseStrength = 150.0f; // regola a piacere
			FVector Impulse = Forward * ImpulseStrength;
			DroppedBackpack->Mesh->AddImpulse(Impulse, NAME_None, true);
		}

		// Chiudi widget inventario se aperto
		if (InventoryWidgetInstance && InventoryWidgetInstance->IsInViewport())
		{
			//InventoryWidgetInstance->RemoveFromParent();
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				PC->SetShowMouseCursor(false);
				PC->SetInputMode(FInputModeGameOnly());
			}
		}

	
	}
}



///////////////////    TORCIA    /////////////////


void APROGETTOCharacter::SetTorchIntensity(float NewIntensity)
{
	if (CharacterTorchSpot)
	{
		CharacterTorchSpot->SetIntensity(NewIntensity);
		UE_LOG(LogTemp, Warning, TEXT("SetTorchIntensity: Intensità aggiornata a %.1f"), NewIntensity);
	}
}

void APROGETTOCharacter::SetTorchColor(const FLinearColor& NewColor)
{
	if (CharacterTorchSpot)
	{
		CharacterTorchSpot->SetLightColor(NewColor);
		UE_LOG(LogTemp, Warning, TEXT("SetTorchColor: Colore aggiornato a R=%.2f G=%.2f B=%.2f"),
			NewColor.R, NewColor.G, NewColor.B);
	}
}

void APROGETTOCharacter::SetTorchRadius(float NewRadius)
{
	if (CharacterTorchSpot)
	{
		CharacterTorchSpot->SetAttenuationRadius(NewRadius);
		UE_LOG(LogTemp, Warning, TEXT("SetTorchRadius: Raggio di attenuazione aggiornato a %.1f"), NewRadius);
	}
}

void APROGETTOCharacter::ToggleTorch()
{
	
	if (!bHasTorchEquipped)
	{
		return;
	}

	if (!bTorchIsOn)
	{
		TurnOnTorch();
	}
	else
	{
		TurnOffTorch();
	}
}

void APROGETTOCharacter::TurnOnTorch()
{
	
	if (CurrentBatteryEnergy <= 0.0f)
	{
		
		return;
	}

	if (bTorchIsOn)
		return;

	bTorchIsOn = true;

	
	if (CharacterTorchSpot)
	{
		CharacterTorchSpot->SetVisibility(true);
	}
	
	GetWorldTimerManager().SetTimer(BatteryTimerHandle, this, &APROGETTOCharacter::DrainBatteryTimerFunction, BatteryDrainInterval, true);
			
}

void APROGETTOCharacter::TurnOffTorch()
{
	if (!bTorchIsOn)
		return;

	bTorchIsOn = false;

	// Spengo la luce sul Character
	if (CharacterTorchSpot)
	{
		CharacterTorchSpot->SetVisibility(false);
	}

	// Ferma il timer di battery drain
	GetWorldTimerManager().ClearTimer(BatteryTimerHandle);
}

void APROGETTOCharacter::DrainBatteryTimerFunction()
{
	// Scala la batteria
	CurrentBatteryEnergy = FMath::Max(0.0f, CurrentBatteryEnergy - BatteryDrainAmount);

	// Aggiorna il widget di batteria (mostra la percentuale)
	if (TorchBatteryWidgetInstance)
	{
		float Percent = (MaxBatteryEnergy > 0.0f) ? (CurrentBatteryEnergy / MaxBatteryEnergy) : 0.0f;
		TorchBatteryWidgetInstance->SetBatteryPercent(Percent);
	}

	// Se la batteria è esaurita, spengo automaticamente
	if (CurrentBatteryEnergy <= 0.0f)
	{
		TurnOffTorch();
		UE_LOG(LogTemp, Warning, TEXT("Batteria torcia esaurita, spengo automaticamente"));
	}
}

void APROGETTOCharacter::SetBatteryEnergy(float NewBattery)
{
	CurrentBatteryEnergy = FMath::Clamp(NewBattery, 0.0f, MaxBatteryEnergy);
	if (TorchBatteryWidgetInstance)
	{
		float Percent = (MaxBatteryEnergy > 0.0f) ? (CurrentBatteryEnergy / MaxBatteryEnergy) : 0.0f;
		TorchBatteryWidgetInstance->SetBatteryPercent(Percent);
	}
}

void APROGETTOCharacter::ShowNotification(const FString& Message, float Duration)
{
	if (!NotificationWidgetClass) return;

	if (!ActiveNotificationWidget)
	{
		ActiveNotificationWidget = CreateWidget<UNotificationWidget>(GetWorld(), NotificationWidgetClass);
		if (ActiveNotificationWidget)
		{
			ActiveNotificationWidget->AddToViewport();
		}
	}

	if (ActiveNotificationWidget)
	{
		ActiveNotificationWidget->SetNotificationText(Message);
		ActiveNotificationWidget->SetVisibility(ESlateVisibility::Visible);

		GetWorldTimerManager().ClearTimer(NotificationTimerHandle);
		GetWorldTimerManager().SetTimer(NotificationTimerHandle, this, &APROGETTOCharacter::HideNotification, Duration, false);
	}
}

void APROGETTOCharacter::HideNotification()
{
	if (ActiveNotificationWidget)
	{
		ActiveNotificationWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APROGETTOCharacter::StartRunning()
{
	if (Stamina > 0.f && !bStaminaSottoSoglia)
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void APROGETTOCharacter::StopRunning()
{
	if (bIsRunning) // solo se stava correndo
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;

		bCanRecoverStamina = false;
		TimeSinceStoppedRunning = 0.0f;
	}
}

void APROGETTOCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APROGETTOCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APROGETTOCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APROGETTOCharacter::Look);
		EnhancedInputComponent->BindAction(IA_TogglePower, ETriggerEvent::Started, this, &APROGETTOCharacter::TogglePowerViaLineTrace);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APROGETTOCharacter::StartRunning);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APROGETTOCharacter::StopRunning);
		EnhancedInputComponent->BindAction(MainInteraction, ETriggerEvent::Started, this, &APROGETTOCharacter::HandleInteraction);

		if (Torch)
		{
			EnhancedInputComponent->BindAction(Torch, ETriggerEvent::Started, this, &APROGETTOCharacter::ToggleTorch);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Torch UInputAction è nullptr: bind non eseguito."));
		}



		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &APROGETTOCharacter::ToggleInventory);
		EnhancedInputComponent->BindAction(DropMyBackpack, ETriggerEvent::Started, this, &APROGETTOCharacter::DropBackpack);
		

	}
}

void APROGETTOCharacter::HandleInteraction()
{
	FVector Start = FollowCamera->GetComponentLocation(); // O usare GetActorLocation()
	FVector Forward = FollowCamera->GetForwardVector();
	FVector End = Start + Forward * 150.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (bHit && Hit.GetActor())
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor->GetClass()->ImplementsInterface(UMainInteractionInterface::StaticClass()))
		{
			IMainInteractionInterface* Interface = Cast<IMainInteractionInterface>(HitActor);
			if (Interface)
			{
				Interface->MainInteract(this);
			}
		}
	}

	// Debug
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f);
}

bool APROGETTOCharacter::EquipItemToSlot(ABaseItem* Item, EEquipmentSlot Slot)
{
	
	
	if (!Item || Item->ItemType != EItemType::Equippable)
	{
		ShowNotification(TEXT("Item non è equipaggiabile"), 1.5f);
		return false;
	}

	
	if (!Item->AllowedEquipmentSlots.Contains(Slot))
	{
		ShowNotification(TEXT("Questo oggetto non può essere equipaggiato qui"), 1.5f);
		return false;
	}

	
	ABaseItem** AlreadyEquipped = EquippedItemSlots.Find(Slot);
	if (AlreadyEquipped && *AlreadyEquipped)
	{
		ShowNotification(TEXT("Slot già occupato"), 1.5f);
		return false;
	}

	
	for (auto It = EquippedItemSlots.CreateIterator(); It; ++It)
	{
		if (It->Value == Item)
		{
			It.RemoveCurrent();
			Item->bIsEquipped = false;
			break;
		}
	}

	
	int32 IndexInInventory = Inventory.Find(Item);
	if (IndexInInventory != INDEX_NONE)
	{
		Inventory.RemoveAtSwap(IndexInInventory);
	}

	
	EquipItem(Item);

	
	EquippedItemSlots.Add(Slot, Item);

	ShowNotification(
		FString::Printf(TEXT("Equipped %s in %s"),
			*Item->ItemName,
			*UEnum::GetValueAsString(Slot)),
		1.5f
	);

	
	if (InventoryWidgetInstance && InventoryWidgetInstance->IsVisible())
	{
		
		InventoryWidgetInstance->SetMyInventoryItems(
			Inventory,
			CurrentCarryWeight,
			MaxCarryWeight
		);
		InventoryWidgetInstance->UpdateEquippedDisplay();
	}

	return true;
}





void APROGETTOCharacter::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth -= DamageAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	UpdateHealthUI();

	UE_LOG(LogTemp, Warning, TEXT("Danno ricevuto: %f, Salute attuale: %f"), DamageAmount, CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		// Eventuale gestione della morte
		UE_LOG(LogTemp, Warning, TEXT("Il personaggio è morto."));
	}
}

void APROGETTOCharacter::UpdateHealthUI()
{
	if (StatsWidgetInstance)
	{
		float HealthPercent = CurrentHealth / MaxHealth;
		float HungerPercent = CurrentHunger / MaxHunger;
		float ThirstPercent = CurrentThirst / MaxThirst;
		StatsWidgetInstance->SetHealthPercent(HealthPercent);
		StatsWidgetInstance->SetHungerPercent(HungerPercent);
		StatsWidgetInstance->SetThirstPercent(ThirstPercent);
	}
}

void APROGETTOCharacter::DecreaseHunger()
{
	if (CurrentHunger <= 0.0f)
	{
		CurrentHunger = 0.0f;

		// Avvia il danno continuo se non è già attivo
		if (!GetWorldTimerManager().IsTimerActive(StarvationDamageTimerHandle))
		{
			StartStarvationDamage();
		}

		UpdateHealthUI();
		return;
	}

	// Fame ancora > 0, riducila normalmente
	CurrentHunger -= 0.030f;
	CurrentHunger = FMath::Clamp(CurrentHunger, 0.0f, MaxHunger);

	// Se la fame è risalita sopra zero, fermiamo il danno da fame
	if (CurrentHunger > 0.0f)
	{
		StopStarvationDamage();
	}

	UpdateHealthUI();
}

void APROGETTOCharacter::StartHungerTimer()
{
	GetWorldTimerManager().SetTimer(HungerTimerHandle, this, &APROGETTOCharacter::DecreaseHunger, 0.030f, true);
}

void APROGETTOCharacter::StopHungerTimer()
{
	GetWorldTimerManager().ClearTimer(HungerTimerHandle);
}

void APROGETTOCharacter::DecreaseThirst()
{
	if (CurrentThirst <= 0.0f)
	{
		CurrentThirst = 0.0f;

		// Avvia danno da disidratazione
		if (!GetWorldTimerManager().IsTimerActive(DehydrationDamageTimerHandle))
		{
			StartDehydrationDamage();
		}

		UpdateHealthUI();
		return;
	}

	// Thirst ancora > 0, riducila normalmente
	CurrentThirst -= 0.025f;
	CurrentThirst = FMath::Clamp(CurrentThirst, 0.0f, MaxThirst);

	// Se la sete è tornata > 0, fermiamo il danno
	if (CurrentThirst > 0.0f)
	{
		StopDehydrationDamage();
	}

	UpdateHealthUI();
}

void APROGETTOCharacter::StartThirstTimer()
{
	GetWorldTimerManager().SetTimer(ThirstTimerHandle, this, &APROGETTOCharacter::DecreaseThirst, 0.25f, true);
}

void APROGETTOCharacter::StopThirstTimer()
{
	GetWorldTimerManager().ClearTimer(ThirstTimerHandle);
}

void APROGETTOCharacter::StartStarvationDamage()
{
	GetWorldTimerManager().SetTimer(
		StarvationDamageTimerHandle,
		this,
		&APROGETTOCharacter::ApplyStarvationDamage,
		1.0f, // ogni quanti secondi fare danno
		true
	);

	
}

void APROGETTOCharacter::StopStarvationDamage()
{
	GetWorldTimerManager().ClearTimer(StarvationDamageTimerHandle);
	
}

void APROGETTOCharacter::ApplyStarvationDamage()
{
	float DamageAmount = 25.0f;

	if (CurrentThirst <= 0.0f)
	{
		DamageAmount *= 2.0f; // raddoppia il danno se anche la sete è 0
	}

	ApplyDamage(DamageAmount);
	UE_LOG(LogTemp, Warning, TEXT("Danno da fame applicato: %f"), DamageAmount);
}

void APROGETTOCharacter::StartDehydrationDamage()
{
	GetWorldTimerManager().SetTimer(
		DehydrationDamageTimerHandle,
		this,
		&APROGETTOCharacter::ApplyDehydrationDamage,
		1.0f, // ogni quanti secondi fare danno da sete
		true
	);

	
}

void APROGETTOCharacter::StopDehydrationDamage()
{
	GetWorldTimerManager().ClearTimer(DehydrationDamageTimerHandle);
	
}

void APROGETTOCharacter::ApplyDehydrationDamage()
{
	float DamageAmount = 35.0f;

	if (CurrentHunger <= 0.0f)
	{
		DamageAmount *= 2.0f; // raddoppia il danno se anche la fame è 0
	}

	ApplyDamage(DamageAmount);
	UE_LOG(LogTemp, Warning, TEXT("Danno da sete applicato: %f"), DamageAmount);
}

void APROGETTOCharacter::RestoreHunger(float Amount)
{
	
	if (Amount <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("RestoreHunger chiamato con Amount <= 0"));
		return;
	}

	CurrentHunger = FMath::Clamp(CurrentHunger + Amount, 0.f, MaxHunger);

	UpdateHealthUI();        
	
	if (StatsWidgetInstance)
	{
		float HungerPercent = (MaxHunger > 0.f) ? (CurrentHunger / MaxHunger) : 0.f;
		StatsWidgetInstance->SetHungerPercent(HungerPercent);
	}
}

void APROGETTOCharacter::RestoreThirst(float Amount)
{
	// Se l’importo è ? 0, non faccio nulla
	if (Amount <= 0.f)
		return;

	// Aggiungo Amount alla sete corrente e clamp tra 0 e MaxThirst
	CurrentThirst = FMath::Clamp(CurrentThirst + Amount, 0.f, MaxThirst);

	// Aggiorno la percentuale nella UI, se il widget esiste
	if (StatsWidgetInstance)
	{
		float ThirstPercent = (MaxThirst > 0.f) ? (CurrentThirst / MaxThirst) : 0.f;
		StatsWidgetInstance->SetThirstPercent(ThirstPercent);
	}
}

void APROGETTOCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APROGETTOCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APROGETTOCharacter::TogglePowerViaLineTrace()
{
	FHitResult Hit;
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 500.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UConsumeEnergy::StaticClass()))
		{
			IConsumeEnergy* Interface = Cast<IConsumeEnergy>(HitActor);
			if (Interface)
			{
				Interface->Execute_TogglePower(HitActor);
			}
		}
	}
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
}


