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
#include "PROGETTO/Components/InventoryComponent.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	void ShowNotification(const FString& Message, float Duration = 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNotificationWidget> NotificationWidgetClass;

	UPROPERTY()
	UNotificationWidget* ActiveNotificationWidget;

	FTimerHandle NotificationTimerHandle;

	void HideNotification();

	APROGETTOCharacter();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;


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

	// Stato attuale della corsa
	bool bIsRunning;

	// Funzioni di input
	void StartRunning();

	void StopRunning();

	void HandleInteraction();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma endregion INPUT


#pragma region INVENTORY

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	UInventoryWidget* InventoryWidgetInstance = nullptr;

	UFUNCTION()
	void ToggleInventory();

#pragma endregion INVENTORY
	

#pragma region STATS

	// Funzione per ripristinare la fame
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RestoreHunger(float Amount);

	// Funzione per ripristinare la sete
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RestoreThirst(float Amount);

	FTimerHandle ThirstTimerHandle;

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

	// Fame
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHunger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHunger;

	// Timer per fame
	FTimerHandle HungerTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StatsWidgetClass;

	// Istanza attiva del widget
	UPROPERTY()
	UStatsWidget* StatsWidgetInstance;

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

	void UpdateHealthUI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float MaxStamina;

	float TimeSinceStoppedRunning;

	bool bCanRecoverStamina;

	bool bStaminaSottoSoglia;

#pragma endregion STATS

};   
