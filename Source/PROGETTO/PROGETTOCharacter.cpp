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
#include "PROGETTO/Widgets/StatsWidget.h"
#include "PROGETTO/Actors/BackpackActor.h"
#include "Components/ProgressBar.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PROGETTO/Structs/Enums/EquipmentTypes.h"
#include "Components/SpotLightComponent.h"
#include "Interfaces/MainInteractionInterface.h"
#include "Components/PointLightComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "PROGETTO/Components/InventoryComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


APROGETTOCharacter::APROGETTOCharacter()
{
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Crea il componente inventario e lo associa al Character
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	
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

}

void APROGETTOCharacter::ToggleInventory()
{
	
	if (InventoryComponent)
	{
		InventoryComponent->MyToggleInventory();
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
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APROGETTOCharacter::StartRunning);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APROGETTOCharacter::StopRunning);
		EnhancedInputComponent->BindAction(MainInteraction, ETriggerEvent::Started, this, &APROGETTOCharacter::HandleInteraction);
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &APROGETTOCharacter::ToggleInventory);
		
		

	}
}

void APROGETTOCharacter::HandleInteraction()
{
	FVector Start = FollowCamera->GetComponentLocation(); 
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



