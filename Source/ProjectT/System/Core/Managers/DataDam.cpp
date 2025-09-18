// Fill out your copyright notice in the Description page of Project Settings.


#include "DataDam.h"

#include "Engine/World.h"
#include "Engine/DataTable.h"

#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"

#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateStructStage.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Settings/GameSettings/CoreGameUserSettings.h"


UDataDam::UDataDam()
{

}

void UDataDam::Initialize()
{
	LoadDataTables();
}

void UDataDam::LoadDataTables()
{
	int8 Max = static_cast<int8>(*TEnumRange<EDataTableKey>().end());
	NMT_CHECKF(Max >= 0);
	
	DataTableArray.Reserve(Max);
	DataTableArray.SetNum(Max);
	
	for (EDataTableKey e : TEnumRange<EDataTableKey>())
	{
		FString DataTableName = JOIN_STRINGS(TEXT(""), TEXT("DT_"), EnumToString(e)); 
		UObject* LoadObject = UAssetUtilsLibrary::LoadDataTableByName(DataTableName);
		UDataTable* LoadDataTable = CastChecked<UDataTable>(LoadObject);
	
		uint8 Index = static_cast<uint8>(e);
	
		if (DataTableArray.IsValidIndex(Index))
			DataTableArray[Index] = LoadDataTable;
	}
	UE_LOG(LogTemp, Display, TEXT("Data Table Loaded"));
}

EDataTableKey UDataDam::GetDataTableKeyByName(const FName& InKeyName)
{
	UEnum* DataTableKeyEnum = StaticEnum<EDataTableKey>();
	if (!NMT_ENSURE(DataTableKeyEnum)) return EDataTableKey();

	for (EDataTableKey EnumValue : TEnumRange<EDataTableKey>())
	{
		const FString& ValueString = DataTableKeyEnum->GetNameStringByValue(StaticCast<int64>(EnumValue));
		
		if (InKeyName == ValueString)
			return EnumValue;
	}
	
	return EDataTableKey();
}

const UDataTable* UDataDam::GetDataTableByEnum(EDataTableKey EArrayKey)
{
	uint8 Idx = static_cast<uint8>(EArrayKey);
	bool bValidArrayIdx = DataTableArray.IsValidIndex(Idx);
	if (!NMT_ENSURE(bValidArrayIdx))	return nullptr;
	return DataTableArray[Idx];
}

FName UDataDam::GetDataTableRowNameByIndex(EDataTableKey EArrayKey, const int32 Index)
{
	const UDataTable* FindTable = GetDataTableByEnum(EArrayKey);
	if (!NMT_ENSURE(FindTable)) return FName();

	TArray<FName> RowNames = FindTable->GetRowNames();
	bool bValidate = !RowNames.IsEmpty() && RowNames.IsValidIndex(Index);
	if (!NMT_ENSURE(bValidate)) return FName();

	return RowNames[Index];
}

int32 UDataDam::GetDataTableIndexByRowName(EDataTableKey EArrayKey, FName RowName)
{
	const UDataTable* FindTable = GetDataTableByEnum(EArrayKey);
	if (!NMT_ENSURE(FindTable)) return 0;

	TArray<FName> RowNames = FindTable->GetRowNames();
	bool bValidate = !RowNames.IsEmpty() && RowNames.Find(RowName);
	if(!NMT_ENSURE(bValidate)) return 0;

	return RowNames.Find(RowName);
}

int32 UDataDam::GetDataTableNumberByRowName(EDataTableKey EArrayKey, FName RowName)
{
	const UDataTable* FindTable = GetDataTableByEnum(EArrayKey);
	if (!NMT_ENSURE(FindTable)) return 0;

	TArray<FName> RowNames = FindTable->GetRowNames();
	bool bValidate = !RowNames.IsEmpty() && RowNames.Find(RowName);
	if(!NMT_ENSURE(bValidate)) return 0;

	return RowNames.Find(RowName) + 1;
}


FStage UDataDam::GetLevelDataByCurrentLevelName(const UObject* InWorldContextObject) const
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(InWorldContextObject);

	TArray<FStage*> Stages = GetDataTableRowsChecked<FStage>();

	for (FStage* Stage : Stages)
	{
		if (Stage->sStageName == CurrentLevelName)
			return *Stage;
	}
	return FStage();
}
