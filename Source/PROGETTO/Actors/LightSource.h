// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumerEnergyActor.h"
#include "Components/PointLightComponent.h"
#include "LightSource.generated.h"

/**
 * 
 */
UCLASS()
class PROGETTO_API ALightSource : public AConsumerEnergyActor
{
	GENERATED_BODY()

public:

	ALightSource();

protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	// Accende/spegne la luce
	virtual void ToggleLight();

	virtual void OnPowerLost() override;

	// PointLight visiva
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLight;

	// Stato della luce
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	bool bIsLightOn;

	// Timer per ritardo accensione
	FTimerHandle LightActivationTimerHandle;

	// Ritardo in secondi prima che la luce si accenda
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light|Delay")
	float LightActivationDelay = 1.0f;

	// Funzione chiamata dopo il ritardo
	UFUNCTION()
	void CompleteLightActivation();

	// === Guasto Luce ===

// Probabilità [0-1] che la luce sia rotta
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light|Failure")
	float FailureChance = 0.3f;

	// Min e Max tempo prima che la luce rotta si spenga
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light|Failure")
	FVector2D BrokenLightLifetimeRange = FVector2D(2.0f, 6.0f);

	// Tempo tra gli sfarfallii (range)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light|Failure")
	FVector2D FlickerIntervalRange = FVector2D(0.05f, 0.3f);

	// Velocità fade out
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light|Failure")
	float FadeOutDuration = 1.0f;

	// Flag interno
	bool bIsBroken = false;

	// Timer vari
	FTimerHandle FlickerTimerHandle;
	FTimerHandle BrokenLightShutdownTimerHandle;
	FTimerHandle FadeOutTimerHandle;

	// === Funzioni ===
	void ScheduleBrokenLightShutdown();
	void Flicker();
	void StartFadeOut();
	void UpdateFadeOut();

	float ElapsedFadeTime = 0.0f;
	float InitialLightIntensity = 0.0f;
	
};
