// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DebugHitCollision.generated.h"

UINTERFACE()
class UDebugHitCollision : public UInterface
{
	GENERATED_BODY()
};

/**
 * NOTE: 인터페이스로 등록된 오브젝트는 CoreCheatManager에 의해서 Collision을 시각화 시킬수 있음
 */
class PROJECTT_API IDebugHitCollision
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DebugHitCollision")
	void ShowVisual();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DebugHitCollision")
	void HideVisual();
	
};
