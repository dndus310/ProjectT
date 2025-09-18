// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LevelDataPathLibrary.generated.h"

namespace LevelDataPaths
{
	extern const FString MultiPlay;
	extern const FString SinglePlay;
	namespace Office
	{
		extern const FString OfficeDir;
	}
}

UCLASS()
class PROJECTT_API ULevelDataPathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
