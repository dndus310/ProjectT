// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 에디터 우측 하단 시스템 알림을 위한 클래스
 */
class EDITORUTILITYMODULE_API MessageAlert
{
public:
	MessageAlert();
	~MessageAlert();

public:
	static void ShowNotifyInfo(const FString& Msg);
};
