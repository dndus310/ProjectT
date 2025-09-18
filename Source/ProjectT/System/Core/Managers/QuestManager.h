// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"
#include "ProjectT/Data/Gen/GenerateEnumContentType.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveSubTypes.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveTypes.h"
#include "ProjectT/Data/Gen/GenerateStructStage.h"
#include "ProjectT/System/Core/Interfaces/ContentType.h"
#include "UObject/Object.h"
#include "QuestManager.generated.h"

enum class EObjectiveSubTypes : uint8;
enum class EObjectiveTypes : uint8;
struct FStageData;
struct FObjective;

USTRUCT(BlueprintType)
struct FObjectiveSubTypeContext
{
	GENERATED_BODY()

	FObjectiveSubTypeContext():	QuestNumber(0),	ObjectiveSubType(){}
	FObjectiveSubTypeContext(const int32 InQuestNumber, const EObjectiveSubTypes InObjectiveSubType) : QuestNumber(InQuestNumber), ObjectiveSubType(InObjectiveSubType) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QuestNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveSubTypes ObjectiveSubType;
};

USTRUCT(BlueprintType)
struct FQuestObjective
{
	GENERATED_BODY()
	
	FQuestObjective(const EObjectiveTypes ObjectiveType, const EObjectiveSubTypes ObjectiveSubType, const int32 ItemIndex,
		const int32 ObjectiveCount, const int32 ObjectiveDescIndex) :
		ObjectiveType(ObjectiveType),
		ObjectiveSubType(ObjectiveSubType),
		ObjectiveIndex(ItemIndex),
		ObjectiveCount(ObjectiveCount),
		ObjectiveDescIndex(ObjectiveDescIndex){}

	FQuestObjective():
		ObjectiveType(),
		ObjectiveSubType(),
		ObjectiveIndex(0),
		ObjectiveCount(0),
		ObjectiveDescIndex(0){}
	
	void DecreaseObjectiveCount();
	bool IsComplete() const;

	EObjectiveTypes GetObjectiveType() const { NMT_CHECKF(ObjectiveType != EObjectiveTypes::None); return ObjectiveType; }
	EObjectiveSubTypes GetObjectiveSubType() const { NMT_CHECKF(ObjectiveSubType != EObjectiveSubTypes::None); return ObjectiveSubType; }
	int32 GetObjectiveIndex() const { return ObjectiveIndex; }

	bool DoesSubTypeMatch(const EObjectiveSubTypes TargetSubType, const int32 ObjectiveTargetIndex = 0) const
	{
		return TargetSubType == ObjectiveSubType && ObjectiveIndex == ObjectiveTargetIndex;	
	}
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EObjectiveTypes ObjectiveType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EObjectiveSubTypes ObjectiveSubType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 ObjectiveIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 ObjectiveCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 ObjectiveDescIndex;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnQuestChangedDynamic, const EObjectiveTypes, ObjectiveTypes, const FObjectiveSubTypeContext, ObjectiveSubTypeContext, const bool, IsQuestCompleted);
DECLARE_DELEGATE_ThreeParams(FOnQuestChangedStatic, const EObjectiveTypes, const FObjectiveSubTypeContext, const bool /* IsQuestCompleted */)

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnQuestInitializedDynamic, const TArray<FObjective>&, QuestList);
DECLARE_DELEGATE_OneParam(FOnQuestInitializedStatic, const TArray<FObjective>& /* QuestList */);

template<typename StaticDelegateType, typename DynamicDelegateType, typename... Params>
struct FIntegrateDelegate
{
	StaticDelegateType StaticDelegate;
	DynamicDelegateType DynamicDelegate;

	FIntegrateDelegate(){}
	FIntegrateDelegate(StaticDelegateType const& Delegate) : StaticDelegate(Delegate){}
	FIntegrateDelegate(DynamicDelegateType const& Delegate) : DynamicDelegate(Delegate){}

	void Execute(Params... Args)
	{
		if (StaticDelegate.IsBound()) StaticDelegate.Execute(Args...);
		else if (DynamicDelegate.IsBound()) DynamicDelegate.Execute(Args...);
		else NMT_MSG_CHECKF(0, "FIntegrateDelegate Does Not Bound!");
	}
	bool IsBound() const
	{
		return StaticDelegate.IsBound() || DynamicDelegate.IsBound();
	}
	void Unbind()
	{
		StaticDelegate.Unbind();
		DynamicDelegate.Unbind();
	}
};

using QuestChangedDelegate = FIntegrateDelegate<FOnQuestChangedStatic, FOnQuestChangedDynamic, const EObjectiveTypes, const FObjectiveSubTypeContext, const bool>;
USTRUCT(BlueprintType)
struct FQuestChangedItem
{
	GENERATED_BODY()
	FQuestChangedItem() : Object(nullptr), Subscribe(nullptr){}
	FQuestChangedItem(const UObject* Object, QuestChangedDelegate const& Subscribe) : Object(Object), Subscribe(Subscribe){}

	bool operator==(const FQuestChangedItem& Other) const
	{
		return Object == Other.Object;
	}

	bool operator!=(const FQuestChangedItem& Other) const
	{
		return !(*this == Other);
	}

	UPROPERTY()
	const UObject* Object;
	QuestChangedDelegate Subscribe;
};

using QuestInitialized = FIntegrateDelegate<FOnQuestChangedStatic, FOnQuestChangedDynamic, const TArray<FQuestObjective>&>;
USTRUCT(BlueprintType)
struct FQuestInitializedItem
{
	GENERATED_BODY()
	FQuestInitializedItem() : Object(nullptr), Subscribe(nullptr){}
	FQuestInitializedItem(const UObject* Object, QuestInitialized const& Subscribe) : Object(Object), Subscribe(Subscribe){}

	bool operator==(const FQuestInitializedItem& Other) const
	{
		return Object == Other.Object;
	}

	bool operator!=(const FQuestInitializedItem& Other) const
	{
		return !(*this == Other);
	}

	UPROPERTY()
	const UObject* Object;
	QuestInitialized Subscribe;
};

UCLASS()
class PROJECTT_API UQuestManager : public UObject, public IContentType
{
	GENERATED_BODY()
	CONTENT_TYPE_NUMBER(EContentType::Objective)
	
public:
	UQuestManager();

	void ReleaseManager();
	void InitializeManager(const UObject* WorldObjectContext);
	void InitializeQuestList(const FStage& StageRowData, const FOnQuestInitializedStatic& Callback);
	void UpdateQuestManager(const UObject* Object, const FOnQuestChangedStatic& Callback);
	bool IsAllClearedSubQuests();

	void AddQuestChangedStatic(const UObject* Object, const FOnQuestChangedStatic& Delegate);
	
	UFUNCTION(BlueprintCallable)
	void AddQuestChangedDynamic(const UObject* Object, const FOnQuestChangedDynamic& Delegate);

	UFUNCTION(BlueprintCallable)
	void CheckObjectiveSatisfied(const EObjectiveSubTypes ObjectiveSubTypes, const int32 ObjectiveTargetIndex = 0);
	
private:
	void InitializeMission(const int32 MissionIndex, const UDataTable* ObjectiveTable);
	void InitializeSubQuest(const FStage& StageRowData, const UDataTable* ObjectiveTable);

	void BroadCastQuestChangedEvent(const EObjectiveTypes ObjectiveTypes, const FObjectiveSubTypeContext ObjectiveSubTypeContext, const bool IsQuestCompleted);
	void AddQuestChanged(const FQuestChangedItem& QuestChangedItem);
	
private:
	friend UCoreCheatManager;
	
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;
	FQuestObjective MissionData;
	TArray<FQuestObjective> SubQuestList;
	TArray<FQuestChangedItem> Subscribers;

	TArray<FObjective> OriginalQuestList;
};
