// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "DayNightWidget.generated.h"

class UTextBlock;
class UDayNightCycleComponent;

UCLASS()

class PROGETTO_API UDayNightWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* TimeText;

    // Widget per mostrare lo stato giorno/notte
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StateText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
    UDayNightCycleComponent* DayNightComponent;

    UPROPERTY(meta = (BindWidget))
    class UImage* StateIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
    UTexture2D* SunIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
    UTexture2D* MoonIcon;
	
};
