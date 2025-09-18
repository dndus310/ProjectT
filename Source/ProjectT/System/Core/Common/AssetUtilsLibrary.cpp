// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetUtilsLibrary.h"

#include "LevelSequence.h"
#include "Particles/ParticleSystem.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Paths/BaseDataPathLibrary.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Paths/Core/CoreDataPathLibrary.h"


UStaticMesh* UAssetUtilsLibrary::GetStaticMeshFromPath(const FString& Path)
{
	UStaticMesh* Ret = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *Path));
	return Ret;
}

USkeletalMesh* UAssetUtilsLibrary::GetSkeletalMeshFromPath(const FString& Path)
{
	USkeletalMesh* Ret = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *Path));
	return Ret;
}

UTexture2D* UAssetUtilsLibrary::GetTexture2DFromPath(const FString& Path)
{
	UTexture2D* Ret = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *Path));
	return Ret;
}

UFXSystemAsset* UAssetUtilsLibrary::GetFXSystemFromPath(const FString& Path)
{
	UFXSystemAsset* Ret = Cast<UFXSystemAsset>(StaticLoadObject(UFXSystemAsset::StaticClass(), nullptr, *Path));
	return Ret;
}

ULevelSequence* UAssetUtilsLibrary::GetLevelSequenceFromPath(const FString& Path)
{
	ULevelSequence* Ret = Cast<ULevelSequence>(StaticLoadObject(ULevelSequence::StaticClass(), nullptr, *Path));
	return Ret;
}

USoundBase* UAssetUtilsLibrary::GetSoundBaseFromPath(const FString& Path)
{
	USoundBase* Ret = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *Path));
	return Ret;
}

USoundAttenuation* UAssetUtilsLibrary::GetSoundAttenuationFromPath(const FString& Path)
{
	USoundAttenuation* Ret = Cast<USoundAttenuation>(StaticLoadObject(USoundAttenuation::StaticClass(), nullptr, *Path));
	return Ret;	
}
UAnimSequence* UAssetUtilsLibrary::GetAnimSequenceFromPath(const FString& Path)
{
	UAnimSequence* Ret = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, *Path));
	return Ret;
}

USoundBase* UAssetUtilsLibrary::GetSoundBaseByName(const FName& InAssetName)
{
	FString SoundFolder = UBaseDataPathLibrary::GetSoundDir();

	FString BGMPath = SoundFolder + TEXT("BGM/") + InAssetName.ToString();
	FString SFXPath = SoundFolder + TEXT("SFX/") + InAssetName.ToString();

	UObject* LoadedObject = StaticLoadObject(USoundBase::StaticClass(), nullptr, *BGMPath);

	if(LoadedObject)
		return Cast<USoundBase>(LoadedObject);
	else
		return Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *SFXPath));
}

FString UAssetUtilsLibrary::AssetReferencePathToClassPath(const FString& Path)
{
	FString Ret, Str;
	Path.Split("\'", nullptr, &Str);
	Ret = Str.LeftChop(1);
	Ret.Append(TEXT("_C"));
	return Ret;
}

UClass* UAssetUtilsLibrary::GetClassFromAssetData(const FAssetData& InAssetData)
{
	FString ObjPath = InAssetData.GetSoftObjectPath().ToString().Append("_C");

	return StaticLoadClass(UObject::StaticClass(), nullptr, *ObjPath, nullptr, LOAD_None, nullptr);
}

UDataTable* UAssetUtilsLibrary::LoadDataTableByName(const FString& DataTableName)
{
	UDataTable* Ret = nullptr;
	FString ReferenceFilePath = JOIN_STRINGS(TEXT(""), CoreDataPaths::Data::DatatableDir, DataTableName, TEXT("."), DataTableName);
	UObject* StaticLoadObj = StaticLoadObject(UDataTable::StaticClass(), nullptr, *ReferenceFilePath);
	NMT_ENSURE(StaticLoadObj);
	Ret = CastChecked<UDataTable>(StaticLoadObj);
	return Ret;
}

UDataTable* UAssetUtilsLibrary::LoadDataTableByEnumKey(const EDataTableKey DataTableEnumKey)
{
	FString TotalName = JOIN_STRINGS(TEXT(""), TEXT("DT_"), EnumToString(DataTableEnumKey));
	FString ReferenceFilePath = JOIN_STRINGS(TEXT(""), CoreDataPaths::Data::DatatableDir, TotalName, TEXT("."), TotalName);
	UObject* StaticLoadObj = StaticLoadObject(UDataTable::StaticClass(), nullptr, *ReferenceFilePath);
	if(!NMT_ENSURE(StaticLoadObj)) return nullptr;
	
	UDataTable* Ret = CastChecked<UDataTable>(StaticLoadObj);
	return Ret;
}

bool UAssetUtilsLibrary::IsValidRowInDataTable(const EDataTableKey DataTableEnumKey, const FString& RowName)
{
	UDataTable* LoadDT = LoadDataTableByEnumKey(DataTableEnumKey);
	bool IsValid = false;

	if (LoadDT != nullptr)
	{
		TArray<FName> RowNameArray = LoadDT->GetRowNames();
		IsValid = RowNameArray.Contains(RowName);
	}

	return IsValid;
}