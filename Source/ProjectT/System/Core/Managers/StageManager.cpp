// Fill out your copyright notice in the Description page of Project Settings.


#include "StageManager.h"
#include "LoadingManager.h"
#include "NotificationCenter.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/Data/Gen/GenerateEnumContentType.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/Data/Gen/GenerateEnumStage.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Paths/BaseDataPathLibrary.h"
#include "ProjectT/System/Core/Interfaces/CoreCameraControl.h"

UStageManager::UStageManager():
	OuterObject(nullptr),
	MaxStageNumber(0)
{
	LobbyPath = JOIN_STRINGS(TEXT(""), BaseDataPaths::LevelsDir, TEXT("L_Lobby.umap"));  
	SelectMapPath = JOIN_STRINGS(TEXT(""), BaseDataPaths::LevelsDir, TEXT("L_SelectMap.umap"));
	IntroPath = JOIN_STRINGS(TEXT(""), BaseDataPaths::LevelsDir, TEXT("L_Intro.umap"));
}

void UStageManager::GetStageData(const FString& LevelName, StageData& OutData) const
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);

	UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GI);
	const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::Stage);

	TMap<FName, uint8*> RowMap = FoundTable->GetRowMap();
	for(TPair<FName, unsigned char*> Pair : RowMap)
	{
		FStage* StageData = reinterpret_cast<FStage*>(Pair.Value);
		NMT_CHECKF(StageData);

		if(StageData->sStageName == LevelName)
		{
			OutData.Value.StageData = *StageData;
			OutData.Key = Pair.Key;
			FString KeyString = Pair.Key.ToString();
			UGlobalUtilsLibrary::DivideString(CONTENT_DIGIT, KeyString);
			OutData.Value.StageProperties.StageNumber = FCString::Atoi(*KeyString);
			return;
		}
	}
	NMT_MSG_CHECKF(0, "StageData not found");
}

/**
 * NOTE: Stage 테이블의 마지막 행 Index를 가공해서 스테이지 넘버로 변환
 * 챕터에 대한 고려가 없어서 챕터가 생긴다면 문제가 생길 가능성있음
 * @return MaxStageNumber 
 */
int32 UStageManager::GetMaxStage() const
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);
	
	UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GI);
	NMT_CHECKF(DD);
	
	const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::Stage);
	TArray<FName> RowNames = FoundTable->GetRowNames();

	FString KeyString = RowNames.Last().ToString();
	UGlobalUtilsLibrary::DivideString(CONTENT_DIGIT, KeyString);
	int32 Ret = FCString::Atoi(*KeyString);

	return Ret;
}

void UStageManager::InitializeManager(const UObject* WorldObjectContext, const FString& LevelName)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);

	StageData StageData;
	GetStageData(LevelName, StageData);
	CurrentStageInfo = StageData.Value;
	CurrentStageInfo.RowName = StageData.Key;
	MaxStageNumber = GetMaxStage();
}

void UStageManager::NextLevelLoad()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);
	
	UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GI);
	NMT_CHECKF(DD);

	FString NextLevelString = "FREE";
	FStageProperties StageProp = CurrentStageInfo.StageProperties;
	do
	{
		if(StageProp.StageNumber >= MaxStageNumber)
		{
			UGameplayStatics::OpenLevel(OuterObject, *LobbyPath);
			return;
		}
		StageProp.StageNumber += 1;
		FName RowName = GetLevelDataRowName(StageProp);
		const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::Stage);
		FStage* FoundData = FoundTable->FindRow<FStage>(RowName, TEXT("FStage"));
		NMT_CHECKF(FoundData);
		NextLevelString = FoundData->sStageName;
		
	} while (CHECK_FREE(NextLevelString));
	
	const EStage NextLevel = UGlobalUtilsLibrary::GetStringToEnum<EStage>(NextLevelString);
	PrepareLevelLoad(NextLevel);
}

void UStageManager::GetStagePath(const EStage StageType, FString& NextStagePath)
{
	FString NextLevelName = UGlobalUtilsLibrary::GetEnumToString<EStage>(StageType);
	
	StageData StageData;
	GetStageData(NextLevelName, StageData);
	NextStagePath = UGlobalUtilsLibrary::ReferencePathToFilePath(StageData.Value.StageData.sStagePath);

	if (!FPackageName::DoesPackageExist(NextStagePath))
		NMT_MSG_CHECKF(0, "Stage package does not exist: %s", *NextStagePath);
}

void UStageManager::PrepareLevelLoad(const EStage StageType, float FadeDuration , float FadeDelay)
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(OuterObject, 0);
	if(!NMT_ENSURE(Player)) return;

	FString NextStagePath;
	GetStagePath(StageType, NextStagePath);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);
	
	UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(GI);
	NMT_CHECKF(NC);
	NC->Post(ENotificationTypes::Level_Transition);

	ICoreCameraControl* CameraControl = Player->FindComponentByInterface<ICoreCameraControl>();
	CameraControl->StartFade(EFadeType::FadeOut, FOnStaticFinishCameraFade::CreateLambda([this, NextStagePath, StageType]()
	{
		UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
		NMT_CHECKF(GI);

		ULoadingManager* LoadingManager = GI->GetSubsystem<ULoadingManager>();
		NMT_CHECKF(LoadingManager);

		LoadingManager->PreLoadingScreen(StageType);
		LoadPackageAsync(NextStagePath, FLoadPackageAsyncDelegate::CreateUObject(this, &UStageManager::PackageLevelLoaded));
	}), FadeDuration, 1.f);

	FLoadPackageAsyncOptionalParams Params;
	
}

void UStageManager::ForceLoadLevel(EStage StageType)
{
	FString NextStagePath;
	GetStagePath(StageType, NextStagePath);

	UGameplayStatics::OpenLevel(OuterObject, *NextStagePath);
}

void UStageManager::PackageLevelLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
{
	switch(Result)
	{
		case EAsyncLoadingResult::Failed:
		{
			NMT_MSG_CHECKF(0, "Failed LoadLevel: %s", *PackageName.ToString());
			break;
		}
		case EAsyncLoadingResult::Succeeded:
		{
			UWorld* World = UGlobalUtilsLibrary::GetValidWorld(OuterObject);
			UGameplayStatics::OpenLevel(World, PackageName);
			break;
		}
		case EAsyncLoadingResult::Canceled:
		{
			NMT_MSG_ENSURE(0, "Canceled LoadLevel: %s", *PackageName.ToString());
			break;
		}
		default: NMT_CHECKF(0);
	}
}

FName UStageManager::GetLevelDataRowName(const FStageProperties StageProperties)
{
	FString ContentType = FROM_INT(StaticCast<uint8>(EContentType::Stage));

	uint8 StageNumber = StaticCast<uint8>(StageProperties.StageNumber);
	FString Stage =	StageNumber < 1000 ? StageNumber < 100 ? StageNumber < 10 ?
		JOIN_STRINGS(TEXT(""), TEXT("000"), FROM_INT(StageNumber)) :
		JOIN_STRINGS(TEXT(""), TEXT("00"), FROM_INT(StageNumber)) :
		JOIN_STRINGS(TEXT(""), TEXT("0"), FROM_INT(StageNumber)) :
		JOIN_STRINGS(TEXT(""), FROM_INT(StageNumber)); 
	
	FName Ret = JOIN_STRINGS(TEXT(""), ContentType, Stage);

	return Ret;
}

void UStageManager::ReleaseManager()
{
	OuterObject = nullptr;
}
