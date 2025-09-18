// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ContentType.generated.h"

#define CONTENT_TYPE_NUMBER(Number) \
public:\
	virtual FString GetContentTypeNumber_Implementation() \
	{ \
		return FROM_INT(StaticCast<uint8>(Number)); \
	} \
	virtual EContentType GetContentType_Implementation() \
	{ \
		return StaticCast<EContentType>(Number); \
	} \
private:

enum class EContentType : uint8;

UINTERFACE()
class UContentType : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API IContentType
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "ContentType"))
	FString GetContentTypeNumber();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "ContentType"))
	EContentType GetContentType();
};
