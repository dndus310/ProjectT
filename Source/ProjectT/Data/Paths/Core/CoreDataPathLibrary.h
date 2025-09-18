// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoreDataPathLibrary.generated.h"

namespace CoreDataPaths
{
	extern const FString DataDir;
	namespace Data
	{
		extern const FString DatatableDir;
	}
} 

UCLASS()
class PROJECTT_API UCoreDataPathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
