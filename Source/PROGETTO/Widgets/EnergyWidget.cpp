// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEnergyWidget::SetEnergyLevel(float EnergyPercent)
{
    if (EnergyProgressBar)
    {
        float Clamped = FMath::Clamp(EnergyPercent, 0.f, 1.f);
        EnergyProgressBar->SetPercent(Clamped);

        // Interpolazione HSV da Rosso (Hue = 0) a Verde (Hue = 120)
        float Hue = FMath::Lerp(0.f, 120.f, Clamped); // 0 = rosso, 120 = verde
        FLinearColor HSVColor = FLinearColor::MakeFromHSV8((uint8)Hue, 255, 255);
        HSVColor.A = 1.0f;

        EnergyProgressBar->SetFillColorAndOpacity(HSVColor);
    }

}

void UEnergyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetEnergyLevel(0.f);
}
