// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorUtilsLibrary.generated.h"

USTRUCT(Blueprintable)
struct FMaterialInfoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> DetectedMaterial = nullptr;	

	UPROPERTY(BlueprintReadOnly)
	int32 VertexInstructions = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 PixelInstructions = 0;
};

USTRUCT(Blueprintable)
struct FMeshInfoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMeshComponent> DetectedMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 PolygonCounts = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 LODCounts = 0;
};

UCLASS()
class PROJECTT_API UEditorUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	static void ExecuteBatchFile(const FString& FilePath);

	UFUNCTION(BlueprintCallable)
	static TSet<FName> FindAllDependenciesObjectPath(UObject* InAssetToFindRef);

	static void RecursivelyFindDependencies(TSet<FName>& AllDependencies, FName InPackageName);

	UFUNCTION(BlueprintCallable)
	static void SetSubLevelTransform(FString SubLevelName, ULevelStreaming* StreamingLevel, FVector Location, float Yaw,
									 bool& bOutSuccess, FString& OutInfoMessage);

	static ULevel* GetSubLevelFromWorld(UWorld* World, FString SubLevelName, bool& bOutSuccess,
										FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable)
	static void GetUsedTexturesInCurrentLevel(TSet<UTexture*>& OutTextures);

	UFUNCTION(BlueprintCallable)
	static void GetUsedMaterialInCurrentLevel(TSet<UMaterialInterface*>& OutMaterials);
	
	UFUNCTION(BlueprintCallable)
	static void GetMaterialInfoInCurrentLevel(TArray<UMaterialInterface*> InMaterials, TArray<FMaterialInfoData>& OutData);

	UFUNCTION(BlueprintCallable)
	static void GetUsedMeshesInCurrentLevel(TSet<UMeshComponent*>& OutMeshes);

	UFUNCTION(BlueprintCallable)
	static void GetMeshInfoInCurrentLevel(TArray<UMeshComponent*> InMeshes, TArray<FMeshInfoData>& OutData);

	UFUNCTION(BlueprintCallable,Category="VSVersion")
	static bool LoadFileToString(FString RelativePathUnderContent,FString&Output);

	UFUNCTION(BlueprintCallable)
	static bool SaveFileToString(FString InSaveText,FString FileName);

	UFUNCTION(BlueprintPure)
	static void GetCSVFilesList(const FString& RelativePathUnderContent, TArray<FString>& OutputFilesList);

	static void ResetComponentToDefaults(UActorComponent* Component);

	UFUNCTION(BlueprintCallable)
	static void ShowMessageAlert(FString Message);

#endif
};
