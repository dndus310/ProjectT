// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDataPathLibrary.h"

namespace BaseDataPaths
{
	const FString RootDir = "/Game/";
	const FString SystemDir = RootDir + "System/";
	const FString CoreDir = SystemDir + "Core/";
	const FString EnvsDir = RootDir + "Envs/";
	const FString CharactersDir = RootDir + "Characters/";
	const FString LevelsDir = RootDir + "Levels/";
	const FString UIDir = RootDir + "UI/";
	const FString FXDir = RootDir + "FX/";
	const FString SoundDir = RootDir + "Sound/";
	const FString ResourcesDir = RootDir + "Resources/";

	namespace Sound
	{
		const FString BGMDir = SoundDir + "BGM/";
		const FString SFXDir = SoundDir + "SFX/";
	}
}

