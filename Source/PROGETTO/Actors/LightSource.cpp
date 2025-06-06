// Fill out your copyright notice in the Description page of Project Settings.


#include "LightSource.h"

ALightSource::ALightSource()
{
	PrimaryActorTick.bCanEverTick = true;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(RootComponent);
	PointLight->SetVisibility(false); // Spenta all'inizio

	bIsLightOn = false;
	
}

void ALightSource::ToggleLight()
{
	bool PreviousPowerState = IsPoweredOn();

	IConsumeEnergy::Execute_TogglePower(this);
	
	// Rileggi lo stato attuale
	bool NewPowerState = IsPoweredOn();

	if (PreviousPowerState != NewPowerState)
	 
	{
		
		if (NewPowerState)
		{
			
			if (!GetWorldTimerManager().IsTimerActive(LightActivationTimerHandle))
			{
				GetWorldTimerManager().SetTimer(LightActivationTimerHandle, this, &ALightSource::CompleteLightActivation, LightActivationDelay, false);
			}
		}
		else
		{
			// Spegni subito se si perde energia
			bIsLightOn = false;
			PointLight->SetVisibility(false);
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ToggleLight: accensione non riuscita."));
	}
}

void ALightSource::BeginPlay()
{
	Super::BeginPlay();

	// Se vuoi forzare questi valori indipendentemente dal Blueprint:
	EnergyConsumptionPerCycle = 0.025f;
	EnergyConsumptionInterval = 1.0f;
}

void ALightSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALightSource::OnPowerLost()
{
	bIsLightOn = false;
	PointLight->SetVisibility(false);
	UE_LOG(LogTemp, Warning, TEXT("Luce spenta per mancanza di energia."));
}

void ALightSource::CompleteLightActivation()
{
	if (IsPoweredOn())
	{
		bIsLightOn = true;
		PointLight->SetVisibility(true);

		// Calcolo probabilità guasto
		if (FMath::FRand() < FailureChance)
		{
			bIsBroken = true;
			ScheduleBrokenLightShutdown();
			UE_LOG(LogTemp, Warning, TEXT("La luce è GUASTA! Si spegnerà tra poco."));
		}
		else
		{
			bIsBroken = false;
		}
	}
}

void ALightSource::ScheduleBrokenLightShutdown()
{
	float Lifetime = FMath::FRandRange(BrokenLightLifetimeRange.X, BrokenLightLifetimeRange.Y);

	// Timer per spegnimento
	GetWorldTimerManager().SetTimer(BrokenLightShutdownTimerHandle, this, &ALightSource::StartFadeOut, Lifetime, false);

	// Timer ricorsivo per sfarfallio
	GetWorldTimerManager().SetTimer(FlickerTimerHandle, this, &ALightSource::Flicker, FMath::FRandRange(FlickerIntervalRange.X, FlickerIntervalRange.Y), false);
}

void ALightSource::Flicker()
{
	if (!bIsLightOn || !bIsBroken) return;

	// Alterna visibilità o intensità
	bool bVisible = PointLight->IsVisible();
	PointLight->SetVisibility(!bVisible);

	// Ripeti dopo un nuovo intervallo
	float NextInterval = FMath::FRandRange(FlickerIntervalRange.X, FlickerIntervalRange.Y);
	GetWorldTimerManager().SetTimer(FlickerTimerHandle, this, &ALightSource::Flicker, NextInterval, false);
}

void ALightSource::StartFadeOut()
{
	GetWorldTimerManager().ClearTimer(FlickerTimerHandle); // stop flicker

	InitialLightIntensity = PointLight->Intensity;
	ElapsedFadeTime = 0.0f;

	// Timer per fade out progressivo
	GetWorldTimerManager().SetTimer(FadeOutTimerHandle, this, &ALightSource::UpdateFadeOut, 0.05f, true);
}

void ALightSource::UpdateFadeOut()
{
	if (!bIsLightOn) return;

	ElapsedFadeTime += 0.05f;
	float Alpha = FMath::Clamp(ElapsedFadeTime / FadeOutDuration, 0.0f, 1.0f);
	PointLight->SetIntensity(FMath::Lerp(InitialLightIntensity, 0.0f, Alpha));

	if (Alpha >= 1.0f)
	{
		bIsLightOn = false;
		PointLight->SetVisibility(false);
		GetWorldTimerManager().ClearTimer(FadeOutTimerHandle);
	}
}
