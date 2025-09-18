// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingManager.h"

#include "AssetCompilingManager.h"
#include "ShaderCompiler.h"
#include "Widgets/SCompoundWidget.h"
#include "Blueprint/UserWidget.h"
#include "MoviePlayer.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumStage.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/LoadableObject.h"
#include "ProjectT/System/Core/Settings/GeneralSettings/CoreLoadingScreenSettings.h"
#include "Widgets/Images/SThrobber.h"

enum class ELoadingScenario : uint8
{
	Ignore = 0,
	Process
};

TMap<EStage, ELoadingScenario> LoadingScenarioMap =
{
	{EStage::None, ELoadingScenario::Ignore},
	{EStage::L_Entry, ELoadingScenario::Process},
	{EStage::L_Lobby, ELoadingScenario::Ignore},
	{EStage::L_Intro, ELoadingScenario::Ignore},
};

class SLoadingScreenSimple : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreenSimple) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
			[
				SNew(SThrobber)
					.Visibility(EVisibility::SelfHitTestInvisible)
			]
			+SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
					.Visibility(EVisibility::SelfHitTestInvisible)
					.Text(FText::FromString(TEXT("Loading...")))
			]
		];
	}
};


ULoadingManager::ULoadingManager():
	HoldLoadingScreenTime(0.f),
	MaxHoldLoadingScreenTime(0.f),
	LoadingWidgetClass(nullptr),
	LoadingScreenWidget(nullptr),
	bPreLoadingFlag(0),
	bStartupLoading(0),
	LoadingState(ELoadingState::None)
{
}

void ULoadingManager::Deinitialize()
{
	Super::Deinitialize();

	FCoreUObjectDelegates::PreLoadMap.Remove(PreLoadMapHandle);
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);

	RemoveLoadingScreen();
	LoadingScreenWidget = nullptr;
	LoadingWidgetClass = nullptr;
	
	OnLoadingStateDynamic.Clear();
	OnLoadingStateStatic = nullptr;

	if(TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
	}

	PreLoadObjects.Empty();
}

void ULoadingManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	PreLoadMapHandle = FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ULoadingManager::StartLoadMap);
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULoadingManager::EndLoadMap);
}

bool ULoadingManager::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void ULoadingManager::RegisterPreLoadableObject(const TScriptInterface<ILoadableObject>& LoadableObject)
{
	PreLoadObjects.Add(LoadableObject.GetObject());
}

void ULoadingManager::UnRegisterPreLoadableObject(const TScriptInterface<ILoadableObject>& LoadableObject)
{
	PreLoadObjects.RemoveSwap(LoadableObject.GetObject());
}

void ULoadingManager::ReceiveLoadingState(const FOnLoadingStateStatic& Callback)
{
	if(!Callback.IsBound())
		NMT_MSG_CHECKF(0, "Callback Delegate Is Not Bound");

	if(!bStartupLoading)
   	{
		Callback.Execute(ELoadingState::End);
		return;    			
   	}
	
	if(!OnLoadingStateStatic.IsBound())
		OnLoadingStateStatic = Callback;
}

void ULoadingManager::StartLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if(WorldContext.OwningGameInstance == GetGameInstance())
	{
		if(GEngine->IsInitialized())
		{
			const FString BaseFilename = FPaths::GetBaseFilename(MapName);
			const EStage CurrentStage = UGlobalUtilsLibrary::GetStringToEnum<EStage>(BaseFilename);
			BeginLoadingScreen(CurrentStage);	
		}
	}
}

void ULoadingManager::PreLoadingScreen(const EStage Stage)
{
	bPreLoadingFlag = true;
	BeginLoadingScreen(Stage);
}

void ULoadingManager::BeginLoadingScreen(const EStage Stage)
{
	ELoadingScenario* Found = LoadingScenarioMap.Find(Stage);
	if(Found)
	{
		if(*Found == ELoadingScenario::Ignore) return;
	}

	const UCoreLoadingScreenSettings* Settings = GetDefault<UCoreLoadingScreenSettings>();
	if(Stage == EStage::L_Entry)
	{
		if(Settings->bEnableStartUpMovie && !Settings->StartUpMoviePath.IsEmpty())
		{
			bStartupLoading = true;
			FLoadingScreenAttributes LoadingScreen;
			LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
			LoadingScreen.bMoviesAreSkippable = true;
			LoadingScreen.MoviePaths.Add(Settings->StartUpMoviePath);
			GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
			GetMoviePlayer()->OnMoviePlaybackFinished().AddWeakLambda(this, [this]()
			{
				RemoveLoadingScreen();
				PropagateLoadingState(ELoadingState::End);
				LoadingState = ELoadingState::None;
				bPreLoadingFlag = false;
			});
			return;
		}
		if(!Settings->bEnableStartUpMovie && Settings->PreLoadingScreenClassPath.IsValid())
		{
			LoadingWidgetPath = Settings->PreLoadingScreenClassPath;
			LoadingWidgetClass = LoadingWidgetPath.TryLoadClass<UUserWidget>();
		}
	}
	else 
	{
		LoadingWidgetPath = Settings->PostLoadingScreenClassPath;
		LoadingWidgetClass = LoadingWidgetPath.TryLoadClass<UUserWidget>();	
	}
	
	if(LoadingState == ELoadingState::None)
	{
		UGameInstance* GI = GetGameInstance();
		NMT_CHECKF(GI);
	
		if(UGameViewportClient* GameViewportClient = GI->GetGameViewportClient())
			GameViewportClient->RemoveAllViewportWidgets();
		
		MaxHoldLoadingScreenTime = Settings->MinimumLoadingScreenTime;
		ShowLoadingScreen();
	}
}

void ULoadingManager::ShowLoadingScreen()
{
	const UCoreLoadingScreenSettings* Settings = GetDefault<UCoreLoadingScreenSettings>();
	if(Settings->bForceLoadingComplete) 
	{
		EndLoadingScreen();
		return;
	}

	UGameInstance* GI = GetGameInstance();
	NMT_CHECKF(GI);

	if(!LoadingWidgetClass)
	{
		LoadingScreenWidget = SNew(SLoadingScreenSimple);
	}
	else
	{
		if(UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*GI, LoadingWidgetClass, NAME_None))
		{
			LoadingScreenWidget = UserWidget->TakeWidget();
		}
		else
		{
			LoadingScreenWidget = SNew(SLoadingScreenSimple);
		}
	}
	
	NMT_CHECKF(LoadingScreenWidget);
	if(UGameViewportClient* GameViewportClient = GI->GetGameViewportClient())
	{
		GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), Settings->ZOrder);
	}

	PropagateLoadingState(ELoadingState::Start);
	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ULoadingManager::UpdateLoadingScreen));
}

bool ULoadingManager::UpdateLoadingScreen(float DeltaTime)
{
	HoldLoadingScreenTime += DeltaTime;

	const bool bHalfTimeDone = (HoldLoadingScreenTime < MaxHoldLoadingScreenTime * 0.5f);
	if(bHalfTimeDone) return true;

	const bool bWithinMaxHold = (HoldLoadingScreenTime < MaxHoldLoadingScreenTime);

	/**
	 * NOTE: GShaderCompilingManager 글로벌 변수는 PIE(PlayInEditor), StandAlone 일때 유효함
	 * 또한 빌드시 이미 World->Level 은 셰이더 컴파일을 기본적으로 프리 캐싱해서 배포하기 때문에 유도할 이유가 없음
	 * 만약 메모리에 아직 올리지않은 에셋단위거나 직접적으로 로드를 해야한다면 FStreamableManager를 통해 비동기로드 설계를 유도해야할 것 같음
	 */
#if WITH_EDITOR
	const bool bCompiling = GShaderCompilingManager->IsCompiling();
#else
	const bool bCompiling = false;
#endif
	
	if(!bCompiling && !bPreLoadingFlag)
	{
		if(!bWithinMaxHold && PreLoadObjects.IsEmpty())
		{
			EndLoadingScreen();
			return false;
		}
		if(HoldLoadingScreenTime > 20.f && !PreLoadObjects.IsEmpty())
		{
			auto FailedObjects = PreLoadObjects.FilterByPredicate([](const TWeakInterfacePtr<ILoadableObject>& Object)
			{
				return Object->HasFinishedLoading();	
			});
			for(TWeakInterfacePtr<ILoadableObject> Obj : FailedObjects)
			{
				NMT_LOGF("Loading process timed out. Inspect the following object(s) still loading: %s", *Obj.GetWeakObjectPtr()->GetName());
			}
			NMT_CHECKF(0);
		}
		
		return true;
	}

	PropagateLoadingState(ELoadingState::Update);
	return true;
}

void ULoadingManager::RemoveLoadingScreen()
{
	if(!LoadingScreenWidget.IsValid()) return;
	if(TickerHandle.IsValid()) FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);

	if(UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient())
	{
		GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		LoadingScreenWidget.Reset();	
	}

	HoldLoadingScreenTime = 0.f;
	MaxHoldLoadingScreenTime = 0.f;
}

void ULoadingManager::EndLoadingScreen()
{
	RemoveLoadingScreen();
	PropagateLoadingState(ELoadingState::End);
	LoadingState = ELoadingState::None;	
}

void ULoadingManager::EndLoadMap(UWorld* World)
{
	if(bPreLoadingFlag)
		bPreLoadingFlag = false;
}

void ULoadingManager::PropagateLoadingState(const ELoadingState NewState)
{
	LoadingState = NewState;

	if(OnLoadingStateDynamic.IsBound())
		OnLoadingStateDynamic.Broadcast(LoadingState);

	if(OnLoadingStateStatic.IsBound())
		OnLoadingStateStatic.Execute(LoadingState);
}