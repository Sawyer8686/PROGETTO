// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsWidget.h"
#include "Components/ProgressBar.h"

void UStatsWidget::SetHealthPercent(float Percent)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(Percent);
    }
}

void UStatsWidget::SetHungerPercent(float Percent)
{
    if (HealthBar)
    {
        HungerBar->SetPercent(Percent);
    }
        
}

void UStatsWidget::SetThirstPercent(float Percent)
{
    if (ThirstBar)
    {
        ThirstBar->SetPercent(Percent);
    }
}

void UStatsWidget::SetStaminaPercent(float Percent)
{
    if (StaminaBar)
    {
        StaminaBar->SetPercent(Percent);
    }
}


