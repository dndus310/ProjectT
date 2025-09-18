// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BaseDataPathLibrary.generated.h"

namespace BaseDataPaths
{
	extern const FString RootDir;
	extern const FString SystemDir;
	extern const FString CoreDir;
	extern const FString EnvsDir;
	extern const FString CharactersDir;
	extern const FString LevelsDir;
	extern const FString UIDir;	
	extern const FString FXDir;
	extern const FString SoundDir;
	extern const FString ResourcesDir;

	namespace Sound
	{
		extern const FString BGMDir;
		extern const FString SFXDir;
	}

}

UCLASS()
class PROJECTT_API UBaseDataPathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "../Sound/"))
	static FString GetSoundDir() { return BaseDataPaths::SoundDir; }

};
