// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define CONTENT_DIGIT 2

#define NOT_IMPLEMENTED_MSG(Str) UE_LOG(LogTemp, Warning, TEXT("Not Implemented %s() -> Line:%d -> Desc:%s"), TEXT(__FUNCTION__), __LINE__, TEXT(Str))
#define NOT_IMPLEMENTED() UE_LOG(LogTemp, Warning, TEXT("Not Implemented %s() -> Line:%d"), TEXT(__FUNCTION__), __LINE__)
#define CHECK_FREE(Str) Str.Equals("FREE")

#define NMT_CHECKF(Object) checkf((Object), TEXT("%s Is Fail. Please Check %s() -> Line:%d"), TEXT(#Object), TEXT(__FUNCTION__), __LINE__)
#define NMT_ENSURE(Object) ensureMsgf((Object), TEXT("%s Is Fail. Please Check %s() -> Line:%d"), TEXT(#Object), TEXT(__FUNCTION__), __LINE__)

#define NMT_MSG_CHECKF(Object, FormatStr, ...) checkf((Object), TEXT(FormatStr), ##__VA_ARGS__)
#define NMT_MSG_ENSURE(Object, FormatStr, ...) ensureMsgf((Object), TEXT(FormatStr), ##__VA_ARGS__)

#define JOIN_STRINGS(Delimiter, ...) (*(FString::Join<TArray<FString>>({__VA_ARGS__}, Delimiter)))

#define FROM_INT(Num) (*FString::FromInt(Num))
#define STRING_TO_INTEGER(Str) (FCString::Atoi(Str))
#define NAME_TO_INTEGER(Name) (FCString::Atoi(Name.ToString()))

#define SHOW_NMT_LOG 1
#define NMT_LOG_TYPE Warning

#ifndef IGN_PARAM
#define IGN_PARAM(X) { (void)X; }
#endif

#define NMT_LOGF(FormatStr, ...)											\
if(SHOW_NMT_LOG)															\
{																			\
	UE_LOG(LogTemp, NMT_LOG_TYPE, TEXT(FormatStr), ##__VA_ARGS__);			\
}

#define NMT_LOG(FormatStr)													\
if(SHOW_NMT_LOG)															\
{																			\
	UE_LOG(LogTemp, NMT_LOG_TYPE, TEXT(FormatStr));							\
}

#ifdef __COUNTER__
#define NMT_DEBUG_PRINT(Duration, FormatStr, ...)							\
if (GEngine)																\
{																			\
	GEngine->AddOnScreenDebugMessage(										\
	uint64(sizeof(__FILE__) + sizeof(__func__) + __LINE__ + __COUNTER__),	\
	Duration,																\
	FColor::Orange,															\
	FString::Printf(TEXT(FormatStr), ##__VA_ARGS__),						\
	false,																	\
	FVector2D(0.9, 0.9));													\
}

#define NMT_DEBUG_PRINT_ONCE(FormatStr, ...)								\
NMT_DEBUG_PRINT(-1.f, FormatStr, ##__VA_ARGS__)

#define NMT_DEBUG_POS_PRINT(Duration, Position, FormatStr,...)				\
if (GEngine)																\
{																			\
	GEngine->AddOnScreenDebugMessage(										\
	uint64(sizeof(__FILE__) + sizeof(__func__) + __LINE__ + __COUNTER__),	\
	Duration,																\
	FColor::Orange,															\
	FString::Printf(TEXT(FormatStr), ##__VA_ARGS__),						\
	false,																	\
	Position																\
	);																		\
}
#define NMT_DEBUG_POS_PRINT_ONCE(Position, FormatStr, ...)					\
NMT_DEBUG_POS_PRINT(-1.f, Position, FormatStr)
	
#endif
