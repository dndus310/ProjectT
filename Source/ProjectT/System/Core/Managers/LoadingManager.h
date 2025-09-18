// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/Data/Gen/GenerateEnumStage.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LoadingManager.generated.h"


template <typename InterfaceType> class TScriptInterface;
class ILoadableObject;

UENUM(BlueprintType)
enum class ELoadingState: uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Start UMETA(DisplayName = "Start"),
	Update UMETA(DisplayName = "Update"),
	End UMETA(DisplayName = "End") 
};

DECLARE_DELEGATE_OneParam(FOnLoadingStateStatic, const ELoadingState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingStateDynamic, const ELoadingState, LoadingState);

UCLASS()
class PROJECTT_API ULoadingManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	ULoadingManager();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	void ReceiveLoadingState(const FOnLoadingStateStatic& Callback);

	UFUNCTION(BlueprintCallable)
	void PreLoadingScreen(const EStage Stage = EStage::None);
	
	UFUNCTION(BlueprintCallable)
	void RegisterPreLoadableObject(const TScriptInterface<ILoadableObject>& LoadableObject);
	
	UFUNCTION(BlueprintCallable)
	void UnRegisterPreLoadableObject(const TScriptInterface<ILoadableObject>& LoadableObject);

protected:
	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
private:
	void StartLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	void BeginLoadingScreen(const EStage Stage);
	void ShowLoadingScreen();
	bool UpdateLoadingScreen(float DeltaTime);
	void EndLoadingScreen();
	void EndLoadMap(UWorld* World);
	void RemoveLoadingScreen();
	void PropagateLoadingState(const ELoadingState NewState);

public:
	UPROPERTY(BlueprintAssignable)
	FOnLoadingStateDynamic OnLoadingStateDynamic;
	
private:
	float HoldLoadingScreenTime;
	float MaxHoldLoadingScreenTime;

	FOnLoadingStateStatic OnLoadingStateStatic;
	
	FDelegateHandle PreLoadMapHandle;
	FDelegateHandle PostLoadMapHandle;
	FDelegateHandle PostCompileHandle;
	FTSTicker::FDelegateHandle TickerHandle;

	FSoftClassPath LoadingWidgetPath;
	TSubclassOf<UUserWidget> LoadingWidgetClass;
	TSharedPtr<SWidget> LoadingScreenWidget;

	uint8 bPreLoadingFlag : 1;
	uint8 bStartupLoading : 1;
	
	ELoadingState LoadingState;
	TArray<TWeakInterfacePtr<ILoadableObject>> PreLoadObjects;
};