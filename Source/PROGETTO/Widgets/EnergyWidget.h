// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnergyWidget.generated.h"

class UProgressBar;

UCLASS()
class PROGETTO_API UEnergyWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void SetEnergyLevel(float EnergyPercent);

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnergyProgressBar;

	virtual void NativeConstruct() override;
	
};
