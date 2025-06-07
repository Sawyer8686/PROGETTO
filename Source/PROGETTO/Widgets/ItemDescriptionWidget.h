#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h" 
#include "ItemDescriptionWidget.generated.h"

class UInventoryWidget;

/**
 *  UItemDescriptionWidget: mostra una finestra con la descrizione testuale di un oggetto
 */
UCLASS()
class PROGETTO_API UItemDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Imposta il testo della descrizione */
	UFUNCTION(BlueprintCallable, Category = "Item Description")
	void SetDescriptionText(const FText& InDescription);

	/** Imposta il widget Inventory padre per deregistrazione */
	UFUNCTION()
	void SetParentInventoryWidget(UInventoryWidget* Parent);

	/** Riferimento al widget Inventory che ha aperto questa descrizione */
	UPROPERTY()
	UInventoryWidget* ParentInventoryWidget = nullptr;

protected:
	/** Questo TextBlock deve esistere nel Widget Blueprint e chiamarsi esattamente "DescriptionText" */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DescriptionText;

	/** Questo Button deve esistere nel Widget Blueprint e chiamarsi esattamente "CloseButton" */
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

	UPROPERTY(meta = (BindWidget))
	class UBorder* BackgroundBorder;

	virtual void NativeConstruct() override;

private:
	/** Callback per chiudere il widget */
	UFUNCTION()
	void OnCloseButtonClicked();
};


