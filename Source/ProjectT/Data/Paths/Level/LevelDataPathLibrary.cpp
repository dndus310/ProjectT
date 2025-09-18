// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDataPathLibrary.h"

#include "ProjectT/Data/Paths/BaseDataPathLibrary.h"

namespace LevelDataPaths
{
	const FString MultiPlay = BaseDataPaths::LevelsDir + "MultiPlay/";
	const FString SinglePlay = BaseDataPaths::LevelsDir + "SinglePlay/";
	namespace Office
	{
		extern const FString SPOfficeDir = SinglePlay + "Office/";
	}
}
