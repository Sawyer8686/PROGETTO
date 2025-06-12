#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompositePartEntryWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROGETTO_API UCompositePartEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Icona della parte */
    UPROPERTY(meta = (BindWidget))
    UImage* PartIcon;

    /** Nome della parte */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PartName;

    /** Inizializza icona e nome */
    UFUNCTION()
    void Setup(const FString& Name, UTexture2D* IconTexture);
};

