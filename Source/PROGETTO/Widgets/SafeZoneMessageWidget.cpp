// Fill out your copyright notice in the Description page of Project Settings.


#include "SafeZoneMessageWidget.h"
#include "Components/TextBlock.h"

void USafeZoneMessageWidget::ShowMessage()
{
    if (SafeZoneMessage)
    {
        SafeZoneMessage->SetText(FText::FromString(TEXT("Sei entrato nel Rifugio, ora sei al sicuro")));
        SafeZoneMessage->SetVisibility(ESlateVisibility::Visible);  // Assicurati che sia visibile
        UE_LOG(LogTemp, Warning, TEXT("Messaggio visibile: %s"), *SafeZoneMessage->GetText().ToString()); 
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SafeZoneMessage è nullo!"));
    }
}
