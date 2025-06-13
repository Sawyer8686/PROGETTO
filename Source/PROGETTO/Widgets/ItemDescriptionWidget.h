#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h" 
#include "ItemDescriptionWidget.generated.h"

class UInventoryWidget;
class ABaseItem;
class UChestWidget;
class APROGETTOCharacter;


UCLASS()
class PROGETTO_API UItemDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UChestWidget* ParentChestWidget;

	
	UFUNCTION(BlueprintCallable, Category = "Item Description")
	void SetDescriptionText(const FText& InDescription);

	
	UFUNCTION()
	void SetParentInventoryWidget(UInventoryWidget* Parent);

	
	UPROPERTY()
	UInventoryWidget* ParentInventoryWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DescriptionText;

	
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

	UPROPERTY(meta = (BindWidget))
	class UBorder* BackgroundBorder;

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCloseButtonClicked();

	UButton* GetCloseButton() const { return CloseButton; }

};


