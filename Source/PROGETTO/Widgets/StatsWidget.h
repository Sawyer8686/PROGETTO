// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatsWidget.generated.h"


UCLASS()
class PROGETTO_API UStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHealthPercent(float Percent);
	// Fame
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "UI")
	void SetHungerPercent(float Percent);

	// Funzione per aggiornare la barra
	UFUNCTION(BlueprintCallable)
	void SetThirstPercent(float Percent);



	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HungerBar;

	// Bind della barra della sete
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ThirstBar;

	// Stamina
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetStaminaPercent(float Percent);

	// Bind della barra della stamina
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

};
