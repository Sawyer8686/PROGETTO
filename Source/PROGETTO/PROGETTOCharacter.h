// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PROGETTO/Widgets/StatsWidget.h"
#include "Containers/Set.h"
#include "Blueprint/UserWidget.h"
#include "PROGETTO/Actors/BaseItem.h"
#include "Widgets/InventoryWidget.h"
#include "PROGETTO/Actors/BackpackActor.h"
#include "PROGETTO/Widgets/EquipSlotSelectionWidget.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "Components/PointLightComponent.h"
#include "PROGETTO/Widgets/NotificationWidget.h"
#include "PROGETTOCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class USpotLightComponent;
class USoundBase;
class UPhysicalMaterial;
class UParticleSystem;



DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APROGETTOCharacter : public ACharacter
{
	GENERATED_BODY()

public:


	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


#pragma region INPUT
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropMyBackpack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MainInteraction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* RunAction;

#pragma endregion INPUT


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* BackpackPickupSound;

	/** Suono riprodotto all'apertura dell'inventario */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* InventoryOpenSound;

	/** Suono riprodotto alla chiusura dell'inventario */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* InventoryCloseSound;

	void HandleInteraction();

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, ABaseItem*> EquippedItemSlots;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipItemToSlot(ABaseItem* Item, EEquipmentSlot Slot);

	UPROPERTY()
	bool bHasBackpack;

	UPROPERTY()
	TArray<ABaseItem*> Inventory;

	UPROPERTY(EditDefaultsOnly)
	int32 InventorySize;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	UInventoryWidget* InventoryWidgetInstance = nullptr;

	
	UFUNCTION()
	void ToggleInventory();

	void GiveBackpack();

	bool AddItemToInventory(ABaseItem* Item);

	/** Funzione per equipaggiare un oggetto Equippable */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItem(ABaseItem* Item);

	/** Rimuove l’item da quello slot e lo rimette in inventario */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipItemFromSlot(EEquipmentSlot Slot);

	void UnequipItem(ABaseItem* Item);

	void ShowNotification(const FString& Message, float Duration = 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNotificationWidget> NotificationWidgetClass;

	UPROPERTY()
	UNotificationWidget* ActiveNotificationWidget;

	FTimerHandle NotificationTimerHandle;

	void HideNotification();

	// Stato attuale della corsa
	bool bIsRunning;

	// Funzioni di input
	void StartRunning();
	void StopRunning();

	APROGETTOCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Salute massima e attuale
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	// Funzione per applicare danno
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ApplyDamage(float DamageAmount);

	// Getter per la salute attuale
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetCurrentHealth() const { return CurrentHealth; }

	// Getter per la salute massima
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMaxHealth() const { return MaxHealth; }

	// Classe del widget da assegnare nel Blueprint
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StatsWidgetClass;

	// Istanza attiva del widget
	UPROPERTY()
	UStatsWidget* StatsWidgetInstance;

	void UpdateHealthUI();
	
	// Fame
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHunger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHunger;

	// Timer per fame
	FTimerHandle HungerTimerHandle;

	// Funzioni
	void DecreaseHunger();
	void StartHungerTimer();
	void StopHungerTimer();

	// Getter opzionali
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetCurrentHunger() const { return CurrentHunger; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMaxHunger() const { return MaxHunger; }

	FTimerHandle StarvationDamageTimerHandle;

	// SETE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxThirst;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentThirst;

	FTimerHandle ThirstTimerHandle;

	// Funzioni sete
	void DecreaseThirst();
	void StartThirstTimer();
	void StopThirstTimer();

	void StartStarvationDamage();
	void StopStarvationDamage();
	void ApplyStarvationDamage();

	FTimerHandle DehydrationDamageTimerHandle;

	void StartDehydrationDamage();
	void StopDehydrationDamage();
	void ApplyDehydrationDamage();

	virtual void Tick(float DeltaTime) override;

	// Stamina variabile e massimo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float MaxStamina;

	float TimeSinceStoppedRunning;
	bool bCanRecoverStamina;

	bool bStaminaSottoSoglia;

	// Peso massimo trasportabile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	float MaxCarryWeight = 50.0f;

	// Peso attuale
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	float CurrentCarryWeight = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class ABackpackActor> BackpackClass;

	UPROPERTY()
	ABackpackActor* DroppedBackpack;

	/** Restituisce la lista di tutti gli oggetti attualmente equipaggiati */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<ABaseItem*> GetEquippedItems() const;

	UFUNCTION()
	void DropBackpack();

	UFUNCTION()
	bool RemoveItemFromInventory(ABaseItem* Item);

	/** Scarta l’item: lo “droppa” nel mondo e lo rimuove dall’inventario */
	UFUNCTION()
	void DropItem(ABaseItem* Item);

	// Funzione per ripristinare la fame
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RestoreHunger(float Amount);

	// Funzione per ripristinare la sete
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RestoreThirst(float Amount);

	// TSet di FName per immagazzinare quali key il player possiede
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TSet<FName> OwnedKeys;

	// Ritorna true se il player ha la chiave con l'ID specificato
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasKey(FName KeyID) const { return OwnedKeys.Contains(KeyID); }

	// Aggiunge la chiave al set
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddKey(FName KeyID) { OwnedKeys.Add(KeyID); }

	/** Rimuove una chiave dall’insieme di chiavi possedute */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveKey(FName KeyID) { OwnedKeys.Remove(KeyID); }

	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	



};

