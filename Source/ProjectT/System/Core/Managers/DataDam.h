// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/ProjectT.h"
#include "DataDam.generated.h"

enum class EDataTableKey : uint8;
enum class EGameLanguage : uint8;
struct FStage;

UCLASS()
class PROJECTT_API UDataDam : public UObject
{
	GENERATED_BODY()
	
public:
	UDataDam();
	
	void Initialize();
	void LoadDataTables();

public:	
	UFUNCTION(BlueprintPure, Category = "DataDam")
	EDataTableKey GetDataTableKeyByName(const FName& InKeyName);
	
	UFUNCTION(BlueprintPure, Category = "DataDam")
	const UDataTable* GetDataTableByEnum(EDataTableKey EArrayKey);
	
	UFUNCTION(BlueprintPure, Category = "DataDam")
	FName GetDataTableRowNameByIndex(EDataTableKey EArrayKey, const int32 Index);
	
	UFUNCTION(BlueprintPure, Category = "DataDam")
	int32 GetDataTableIndexByRowName(EDataTableKey EArrayKey, FName RowName);
	
	UFUNCTION(BlueprintPure, Category = "DataDam")
	int32 GetDataTableNumberByRowName(EDataTableKey EArrayKey, FName RowName);
	
	UFUNCTION(BlueprintPure, meta = (Category = "DataDam"))
	FStage GetLevelDataByCurrentLevelName(const UObject* InWorldContextObject) const;

	template<typename StructType>
	StructType* GetDataTableRowChecked(const FName& RowName) const
	{
		int32 Index = DataTableArray.IndexOfByPredicate([&](const UDataTable* Item)
		{
			return Item->RowStruct == StructType::StaticStruct();
		});
		bool IsValidIndex = Index >= 0 && Index < DataTableArray.Num();
		NMT_CHECKF(IsValidIndex);
		StructType* FoundRow = DataTableArray[Index]->FindRow<StructType>(RowName, TEXT("Not Found"));

		NMT_MSG_CHECKF(FoundRow, "%s have problem", *RowName.ToString());
		return FoundRow;
	}
	template<typename StructType>
	StructType* GetDataTableRowUnChecked(const FName& RowName) const
	{
		int32 Index = DataTableArray.IndexOfByPredicate([&](const UDataTable* Item)
		{
			return Item->RowStruct == StructType::StaticStruct();
		});
		if(!NMT_ENSURE(Index >= 0 && Index < DataTableArray.Num())) return nullptr;
		
		return DataTableArray[Index]->FindRow<StructType>(RowName, TEXT("Not Found"));
	}
	template<typename StructType>
	TArray<StructType*> GetDataTableRowsChecked() const
	{
		TArray<StructType*> Ret;
		int32 Index = DataTableArray.IndexOfByPredicate([&](const UDataTable* Item)
		{
			bool IsValidItem = IsValid(Item);
			NMT_CHECKF(IsValidItem);
			return Item->RowStruct == StructType::StaticStruct();
		});
		bool IsValidIndex = Index >= 0 && Index < DataTableArray.Num();
		NMT_CHECKF(IsValidIndex);

		if(DataTableArray.IsValidIndex(Index))
			DataTableArray[Index]->GetAllRows<StructType>(StructType::StaticStruct()->GetName(), Ret);

		return Ret;
	}
	template<typename StructType>
	TArray<FName> GetDataTableRowNamesChecked() const
	{
		int32 Index = DataTableArray.IndexOfByPredicate([&](const UDataTable* Item)
		{
			return Item->RowStruct == StructType::StaticStruct();
		});
		bool IsValidIndex = Index >= 0 && Index < DataTableArray.Num();
		NMT_CHECKF(IsValidIndex);
		
		return DataTableArray[Index]->GetRowNames();
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataDam", meta = (AllowPrivateAccess = "true"))
	TArray<UDataTable*> DataTableArray;

};
