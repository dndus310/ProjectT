// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageAlert.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

MessageAlert::MessageAlert()
{
}

MessageAlert::~MessageAlert()
{
}

void MessageAlert::ShowNotifyInfo(const FString& Msg)
{
	FNotificationInfo NotifyInfo(FText::FromString(Msg));

	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 5.f;

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}
