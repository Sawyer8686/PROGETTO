// Fill out your copyright notice in the Description page of Project Settings.


#include "DayNightWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PROGETTO/Components/DayNightCycleComponent.h"

void UDayNightWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (DayNightComponent && TimeText)
    {
        float TimeOfDay = 1 - DayNightComponent->CurrentTimeOfDay; // 0.0 - 1.0
        float TotalMinutes = TimeOfDay * 24.f * 60.f;

        TotalMinutes = FMath::Fmod(TotalMinutes + 360.f, 1440.f);

        int32 Hours = FMath::FloorToInt(TotalMinutes / 60.f);
        int32 Minutes = FMath::FloorToInt(FMath::Fmod(TotalMinutes, 60.f));

        FString TimeString = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
        TimeText->SetText(FText::FromString(TimeString));
    }

    // Aggiorna testo "Giorno" / "Notte"
    if (StateText)
    {
        FString StateString = DayNightComponent->bIsDay ? TEXT("Giorno") : TEXT("Notte");
        StateText->SetText(FText::FromString(StateString));
    }

    if (StateIcon)
    {
        UTexture2D* NewIcon = DayNightComponent->bIsDay ? SunIcon : MoonIcon;

        if (NewIcon)
        {
            StateIcon->SetBrushFromTexture(NewIcon, true);
        }
    }
}
