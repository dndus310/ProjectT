// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreDataPathLibrary.h"
#include "ProjectT/Data/Paths/BaseDataPathLibrary.h"

namespace CoreDataPaths
{
	const FString DataDir = BaseDataPaths::CoreDir + "Data/";
	
	namespace Data
	{
		const FString DatatableDir = DataDir + "DataTable/";
	}
} 

