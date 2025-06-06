// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationWidget.h"
#include "Components/TextBlock.h"

void UNotificationWidget::SetNotificationText(const FString& Message)
{
    if (MessageText)
    {
        MessageText->SetText(FText::FromString(Message));
    }
}
