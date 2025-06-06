#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackpackStatusWidget.generated.h"

UCLASS()
class PROGETTO_API UBackpackStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BackpackStatusText;

	virtual void NativeConstruct() override;

	void SetBackpackStatusVisible(bool bVisible);


};
