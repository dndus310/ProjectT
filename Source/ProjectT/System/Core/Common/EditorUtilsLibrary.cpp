// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorUtilsLibrary.h"

#include "GlobalUtilsLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ProjectT/ProjectT.h"

#if WITH_EDITOR
#include "Engine/Level.h"
#include "LevelUtils.h"
#include "UnrealEd.h" 

#include "MaterialEditingLibrary.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "EditorUtilityModule/SystemMessage/MessageAlert.h"
#endif


#if WITH_EDITOR
void UEditorUtilsLibrary::ExecuteBatchFile(const FString& FilePath)
{
	NMT_CHECKF(FPaths::FileExists(FilePath));
	FString RelativePath = FPaths::ConvertRelativePathToFull(FilePath);
	auto Handle = FPlatformProcess::CreateProc(*RelativePath, nullptr, true, false, false, nullptr, 0, nullptr,
	                                           nullptr);

	NMT_CHECKF(Handle.IsValid());
	FPlatformProcess::CloseProc(Handle);
}

TSet<FName> UEditorUtilsLibrary::FindAllDependenciesObjectPath(UObject* InAssetToFindRef)
{
	if(!InAssetToFindRef) return TSet<FName>();

	TSet<FName> RefPaths;

	FName PackageName = FName(*InAssetToFindRef->GetPackage()->GetName());
	RecursivelyFindDependencies(RefPaths, PackageName);

	return RefPaths;
}
void UEditorUtilsLibrary::RecursivelyFindDependencies(TSet<FName>& AllDependencies, FName InPackageName)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FName> TempArray;
	AssetRegistryModule.Get().GetDependencies(InPackageName, TempArray);

	if(TempArray.Num() == 0) return;

	for(const FName& NameElem : TempArray)
	{
		AllDependencies.Add(NameElem);

		RecursivelyFindDependencies(AllDependencies, NameElem);
	}
}

void UEditorUtilsLibrary::SetSubLevelTransform(FString SubLevelName, ULevelStreaming* StreamingLevel, const FVector Location,
                                               float Yaw, bool& bOutSuccess, FString& OutInfoMessage)
{
	UWorld* World = GEditor->GetEditorWorldContext().World();
	ULevel* Sublevel = GetSubLevelFromWorld(World, SubLevelName, bOutSuccess, OutInfoMessage);

	if(Sublevel == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sublevel is null"))
		return;
	}

	if(StreamingLevel == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("StreamingLevel is null"))
		return;
	}

	FLevelUtils::SetEditorTransform(StreamingLevel, FTransform(FRotator(0.0f, Yaw, 0.0f), Location));

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Set transforming sublevels Succeeded - %s"), *SubLevelName);
}

ULevel* UEditorUtilsLibrary::GetSubLevelFromWorld(UWorld* World, FString SubLevelName, bool& bOutSuccess,
                                                  FString& OutInfoMessage)
{
	if(World == nullptr)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("GetSublevelfromWorld failed - %s"), *SubLevelName);
		return nullptr;
	}

	ULevel* Sublevel = nullptr;

	for(ULevel* PotentialLevel : World->GetLevels())
	{
		Sublevel = PotentialLevel;
		break;
	}

	if(Sublevel == nullptr)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("GetSublevelfromWorld failed - %s"), *SubLevelName);
		return nullptr;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("GetSublevelfromWorld Succeeded - %s"), *SubLevelName);
	return Sublevel;
}

void UEditorUtilsLibrary::GetUsedTexturesInCurrentLevel(TSet<UTexture*>& OutTextures)
{
	TSet<UTexture*> UniqueTextures;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		TArray<UMeshComponent*> MeshComponents;
		Actor->GetComponents<UMeshComponent>(MeshComponents);

		for (UMeshComponent* MeshComp : MeshComponents)
		{
			int32 MatCount = MeshComp->GetNumMaterials();
			for (int32 i = 0; i < MatCount; ++i)
			{
				UMaterialInterface* Mat = MeshComp->GetMaterial(i);
				if (Mat)
				{
					TArray<UTexture*> Textures;
					Mat->GetUsedTextures(Textures, EMaterialQualityLevel::High, true, ERHIFeatureLevel::SM5, true);
					for (UTexture* Tex : Textures)
					{
						UniqueTextures.Add(Tex);
					}
				}
			}
		}
	}
	
	OutTextures = UniqueTextures;

}

void UEditorUtilsLibrary::GetUsedMaterialInCurrentLevel(TSet<UMaterialInterface*>& OutMaterials)
{
	TSet<UMaterialInterface*> UniqueMaterials;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		TArray<UMeshComponent*> MeshComponents;
		Actor->GetComponents<UMeshComponent>(MeshComponents);

		for (UMeshComponent* MeshComp : MeshComponents)
		{
			int32 MatCount = MeshComp->GetNumMaterials();
			for (int32 i = 0; i < MatCount; ++i)
			{
				UMaterialInterface* Mat = MeshComp->GetMaterial(i);
				if (Mat)
				{
					UniqueMaterials.Add(Mat);
				}
			}
		}
	}

	OutMaterials = UniqueMaterials;
}

void UEditorUtilsLibrary::GetMaterialInfoInCurrentLevel(TArray<UMaterialInterface*> InMaterials, TArray<FMaterialInfoData>& OutData)
{
	TSet<UMaterialInterface*> DetectMaterials;
	FMaterialInfoData TempMaterialInfo;

	GetUsedMaterialInCurrentLevel(DetectMaterials);
	InMaterials = DetectMaterials.Array();	
	
	for (int i = 0; i < InMaterials.Num(); i++)
	{
		TempMaterialInfo.DetectedMaterial = InMaterials[i];
		TempMaterialInfo.PixelInstructions = UMaterialEditingLibrary::GetStatistics(InMaterials[i]).NumPixelShaderInstructions;
		TempMaterialInfo.VertexInstructions = UMaterialEditingLibrary::GetStatistics(InMaterials[i]).NumVertexShaderInstructions;

		OutData.Emplace(TempMaterialInfo);
	}
	
	OutData.Sort([](const FMaterialInfoData& A, const FMaterialInfoData& B) {

		return A.PixelInstructions > B.PixelInstructions;

		});	
}

void UEditorUtilsLibrary::GetUsedMeshesInCurrentLevel(TSet<UMeshComponent*>& OutMeshes)
{
	TSet<UMeshComponent*> UniqueMeshes;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		TArray<UMeshComponent*> MeshComponents;
		Actor->GetComponents<UMeshComponent>(MeshComponents);

		for (UMeshComponent* MeshComp : MeshComponents)
		{
			UniqueMeshes.Add(MeshComp);			
		}
	}

	OutMeshes = UniqueMeshes;
}

void UEditorUtilsLibrary::GetMeshInfoInCurrentLevel(TArray<UMeshComponent*> InMeshes, TArray<FMeshInfoData>& OutData)
{
	TSet<UMeshComponent*> DetectMeshes;
	FMeshInfoData TempMeshInfo;

	GetUsedMeshesInCurrentLevel(DetectMeshes);
	InMeshes = DetectMeshes.Array();

	for (int i = 0; i < InMeshes.Num(); i++)
	{
		TempMeshInfo.DetectedMesh = InMeshes[i];
		
		if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(InMeshes[i]))
		{
			if (SMC->GetStaticMesh() != nullptr)
			{
				TempMeshInfo.PolygonCounts = SMC->GetStaticMesh()->GetRenderData()->LODResources[0].GetNumTriangles();
				TempMeshInfo.LODCounts = SMC->GetStaticMesh()->GetRenderData()->LODResources.Num();
			}
			else
			{
				TempMeshInfo.PolygonCounts = 0;
				TempMeshInfo.LODCounts = 0;
			}
		}

		if (USkeletalMeshComponent* SKC = Cast<USkeletalMeshComponent>(InMeshes[i]))
		{
			if (SKC->GetSkeletalMeshAsset() != nullptr)
			{
				TempMeshInfo.PolygonCounts = SKC->GetSkeletalMeshAsset()->GetResourceForRendering()->LODRenderData[0].GetTotalFaces();
				TempMeshInfo.LODCounts = SKC->GetSkeletalMeshAsset()->GetResourceForRendering()->LODRenderData.Num();
			}
			else
			{
				TempMeshInfo.PolygonCounts = 0;
				TempMeshInfo.LODCounts = 0;
			}
		}		

		OutData.Emplace(TempMeshInfo);
	}

	OutData.Sort([](const FMeshInfoData& A, const FMeshInfoData& B) {

		return A.PolygonCounts > B.PolygonCounts;

		});
}

bool UEditorUtilsLibrary::LoadFileToString(FString RelativePathUnderContent, FString& Output)
{
	return FFileHelper::LoadFileToString(Output, *(FPaths::ProjectDir() + "Content\\" + RelativePathUnderContent));
}

bool UEditorUtilsLibrary::SaveFileToString(FString InSaveText, FString FilePath)
{
	InSaveText.ReplaceInline(TEXT("\r"),TEXT(""));
	InSaveText.ReplaceInline(TEXT("\n"),TEXT("\r\n"));
	return FFileHelper::SaveStringToFile(InSaveText,*(FPaths::ProjectDir() + "Content\\" + FilePath), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

void UEditorUtilsLibrary::GetCSVFilesList(const FString& RelativePathUnderContent, TArray<FString>& OutputFilesList)
{
	FString FullPath = FPaths::ProjectDir() + "Content\\" + RelativePathUnderContent + TEXT("\\*.csv");
	IFileManager::Get().FindFiles(OutputFilesList, *FullPath, true, false);
}

void UEditorUtilsLibrary::ResetComponentToDefaults(UActorComponent* Component)
{
	NMT_CHECKF(Component);

	UObject* CDO = Component->GetClass()->GetDefaultObject();
	for (TFieldIterator<FProperty> It(Component->GetClass()); It; ++It)
	{
		FProperty* Property = *It;
		if (!Property->HasAnyPropertyFlags(CPF_Edit | CPF_BlueprintVisible)) continue;
		void* Dest = Property->ContainerPtrToValuePtr<void>(Component);
		void* Src = Property->ContainerPtrToValuePtr<void>(CDO);
		Property->CopyCompleteValue(Dest, Src);
	}
	Component->ReregisterComponent();
}
/*Only Editor Function!*/
void UEditorUtilsLibrary::ShowMessageAlert(FString Message)
{
	MessageAlert::ShowNotifyInfo(Message);
}

#endif
