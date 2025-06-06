// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SafeZoneMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROGETTO_API USafeZoneMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    void ShowMessage();

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* SafeZoneMessage;
	
};
