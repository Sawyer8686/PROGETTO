// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PROGETTO/Components/DayNightCycleComponent.h"
#include "PROGETTO/Widgets/DayNightWidget.h"
#include "PROGETTOGameMode.generated.h"

class UDayNightCycleComponent;
class UDayNightWidget;

UCLASS(minimalapi)
class APROGETTOGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APROGETTOGameMode();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnDayStartHandler();

	UFUNCTION()
	void OnNightStartHandler();

	UPROPERTY()
	UDayNightWidget* DayNightWidget;

	UPROPERTY()
	UDayNightCycleComponent* DayNightCycleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDayNightWidget> DayNightWidgetClass;

	// Metodo di accesso al componente
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	UDayNightCycleComponent* GetDayNightCycleComponent() const { return DayNightCycleComponent; }


protected:

	virtual void BeginPlay() override;
};



