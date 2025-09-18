// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetUtilsLibrary.generated.h"

enum class EDataTableKey: uint8;
UCLASS()
class PROJECTT_API UAssetUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UClass* GetClassFromAssetData(const FAssetData& InAssetData);

	UFUNCTION(BlueprintCallable)
	static class UDataTable* LoadDataTableByName(const FString& DataTableName);

	UFUNCTION(BlueprintCallable)
	static class UDataTable* LoadDataTableByEnumKey(EDataTableKey DataTableEnumKey);

	UFUNCTION(BlueprintCallable)
	static bool IsValidRowInDataTable(const EDataTableKey DataTableEnumKey, const FString& RowName);

	UFUNCTION(Blueprintpure)
	static class UStaticMesh* GetStaticMeshFromPath(const FString& Path);

	UFUNCTION(BlueprintPure)
	static class USkeletalMesh* GetSkeletalMeshFromPath(const FString& Path);

	UFUNCTION(BlueprintPure)
	static class UTexture2D* GetTexture2DFromPath(const FString& Path);

	UFUNCTION(BlueprintPure)
	static class UFXSystemAsset* GetFXSystemFromPath(const FString& Path);

	UFUNCTION(BlueprintPure)
	static class ULevelSequence* GetLevelSequenceFromPath(const FString& Path);

	UFUNCTION(BlueprintPure)
	static class USoundBase* GetSoundBaseFromPath(const FString& Path);

	UFUNCTION(BlueprintPure)
	static class USoundAttenuation* GetSoundAttenuationFromPath(const FString& Path);
	
	UFUNCTION(BlueprintPure)
	static class UAnimSequence* GetAnimSequenceFromPath(const FString& Path);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Category = "Sound"))
	static class USoundBase* GetSoundBaseByName(const FName& InAssetName);

	UFUNCTION(BlueprintPure)
	static FString AssetReferencePathToClassPath(const FString& Path);
	
	template<typename T>
	static T* GetStaticLoadObjectChecked(const FString& Path)
	{
		T* Ret = CastChecked<T>(StaticLoadObject(T::StaticClass(), nullptr, *Path));
		return Ret;
	}

	template<typename T>
	static T* GetStaticLoadObjectUnChecked(const FString& Path)
	{
		T* Ret = Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, *Path));
		return Ret;
	}
};
