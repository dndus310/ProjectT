// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"
#include "DataDam.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveSubTypes.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveTypes.h"
#include "ProjectT/Data/Gen/GenerateStructObjective.h"
#include "ProjectT/Data/Gen/GenerateStructStage.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"

void FQuestObjective::DecreaseObjectiveCount()
{
	if(ObjectiveCount > 0) ObjectiveCount -= 1;
}

bool FQuestObjective::IsComplete() const
{
	return ObjectiveCount <= 0;
}

UQuestManager::UQuestManager() :
	OuterObject(nullptr)
{
}

void UQuestManager::InitializeMission(const int32 MissionIndex, const UDataTable* ObjectiveTable)
{
	FObjective* ObjectiveData = ObjectiveTable->FindRow<FObjective>(FROM_INT(MissionIndex), TEXT("Objective"));
	NMT_MSG_CHECKF(ObjectiveData, "does not found Objective rowname[%s]", FROM_INT(MissionIndex));
	
	OriginalQuestList.Push(*ObjectiveData);
	
	EObjectiveTypes ObjectiveTypes = UGlobalUtilsLibrary::GetStringToEnum<EObjectiveTypes>(ObjectiveData->sObjectiveType);
	EObjectiveSubTypes ObjectiveSubTypes = UGlobalUtilsLibrary::GetStringToEnum<EObjectiveSubTypes>(ObjectiveData->sObjectiveSubType);
	MissionData = FQuestObjective(ObjectiveTypes, ObjectiveSubTypes, ObjectiveData->iObjectiveItemId, ObjectiveData->iObjectiveCount, ObjectiveData->iObjectiveDescId);
}

void UQuestManager::InitializeSubQuest(const FStage& StageRowData, const UDataTable* ObjectiveTable)
{
	UScriptStruct* ScriptStruct = StageRowData.StaticStruct();
	TFieldIterator<FProperty> It = ScriptStruct;
	for(++It; It; ++It)
	{
		FProperty* Property = *It;
		NMT_CHECKF(Property);

		const FName PropName = *Property->GetName();
		FName TargetLimitName = GET_MEMBER_NAME_CHECKED(FStage, iTimeLimit);
		if(PropName.IsEqual(TargetLimitName)) break;
		
		FString ValueStr;
		Property->ExportText_InContainer(0, ValueStr, &StageRowData, &StageRowData, nullptr, PPF_None);
		int32 FieldIdx = FCString::Atoi(*ValueStr);
		if(FieldIdx > 0)
		{
			FObjective* ObjectiveData = ObjectiveTable->FindRow<FObjective>(FROM_INT(FieldIdx), TEXT("Objective"));
			NMT_MSG_CHECKF(ObjectiveData, "does not found Objective rowname[%s]", FROM_INT(FieldIdx));

			/** NOTE: iObjectiveCount가 '0' 이라면 해당 타입의 퀘스트를 사용하지않음 */
			if(ObjectiveData->iObjectiveCount == 0) continue;

			OriginalQuestList.Push(*ObjectiveData);
			EObjectiveTypes ObjectiveTypes = UGlobalUtilsLibrary::GetStringToEnum<EObjectiveTypes>(ObjectiveData->sObjectiveType);
			EObjectiveSubTypes ObjectiveSubTypes = UGlobalUtilsLibrary::GetStringToEnum<EObjectiveSubTypes>(ObjectiveData->sObjectiveSubType);
			FQuestObjective Data = FQuestObjective(ObjectiveTypes, ObjectiveSubTypes, ObjectiveData->iObjectiveItemId, ObjectiveData->iObjectiveCount, ObjectiveData->iObjectiveDescId);
			SubQuestList.Push(Data);
		}
	}
}

void UQuestManager::InitializeQuestList(const FStage& StageRowData, const FOnQuestInitializedStatic& Callback)
{
	if(StageRowData.IsDefault() || StageRowData.iMissionId == 0)
	{
		if(Callback.IsBound()) Callback.Execute(TArray<FObjective>{});
		return;
	}

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(OuterObject);
	UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
	NMT_CHECKF(GI);

	UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GI);
	const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::Objective);
	
	InitializeMission(StageRowData.iMissionId, FoundTable);
	InitializeSubQuest(StageRowData, FoundTable);

	if(Callback.IsBound())
		Callback.Execute(OriginalQuestList);
}

void UQuestManager::UpdateQuestManager(const UObject* Object, const FOnQuestChangedStatic& Callback)
{
	AddQuestChangedStatic(Object, Callback);
}

bool UQuestManager::IsAllClearedSubQuests()
{
	bool bSubQuestEmpty = SubQuestList.IsEmpty();
	if(!NMT_MSG_ENSURE(!bSubQuestEmpty, "SubQuest list is empty")) return false;
	
	int32 SubQuestClearCount = 0;
	for(const FQuestObjective& SubQuest : SubQuestList)
	{
		if(!SubQuest.IsComplete() && SubQuest.GetObjectiveSubType() == EObjectiveSubTypes::Avoid)
			++SubQuestClearCount;
		
		else if(SubQuest.IsComplete() && SubQuest.GetObjectiveSubType() != EObjectiveSubTypes::Avoid)
			++SubQuestClearCount;
	}
	
	return SubQuestClearCount == SubQuestList.Num();
}

void UQuestManager::ReleaseManager()
{
	OuterObject = nullptr;

	for(auto Item : Subscribers)
	{
		if(Item.Subscribe.IsBound())
			Item.Subscribe.Unbind();

		if(Item.Object)
			Item.Object = nullptr;
	}
	
	Subscribers.Empty();
	SubQuestList.Empty();
}

void UQuestManager::InitializeManager(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);
}

/**
 * @param ObjectiveSubTypes SubQuest 유형별 타입입니다. 
 * @param ObjectiveTargetIndex TargetIndex는 'ObjectiveSubTypes' 유형에 따라 없을수도 있습니다 따라서 Index가 없다면 0을 보내면 조건에 만족합니다.  
 */
void UQuestManager::CheckObjectiveSatisfied(const EObjectiveSubTypes ObjectiveSubTypes, const int32 ObjectiveTargetIndex)
{
	switch(ObjectiveSubTypes)
	{
		case EObjectiveSubTypes::Save: case EObjectiveSubTypes::Collect: case EObjectiveSubTypes::Avoid: case EObjectiveSubTypes::Kill:
		{
			if(MissionData.GetObjectiveSubType() == ObjectiveSubTypes)
			{
				MissionData.DecreaseObjectiveCount();

				if(MissionData.IsComplete())
					BroadCastQuestChangedEvent(EObjectiveTypes::Mission, FObjectiveSubTypeContext(0, ObjectiveSubTypes), true);
				else
					BroadCastQuestChangedEvent(EObjectiveTypes::Mission, FObjectiveSubTypeContext(0, ObjectiveSubTypes), false);
				
				break;
			}
			
			for(auto It = SubQuestList.CreateIterator(); It;)
			{
				if(It->DoesSubTypeMatch(ObjectiveSubTypes, ObjectiveTargetIndex))
				{
					if(It->IsComplete()) break;
					
					It->DecreaseObjectiveCount();
					if(It->IsComplete())
					{
						BroadCastQuestChangedEvent(EObjectiveTypes::Quest, FObjectiveSubTypeContext(It.GetIndex(), ObjectiveSubTypes), true);
						break;
					}
					else BroadCastQuestChangedEvent(EObjectiveTypes::Quest, FObjectiveSubTypeContext(It.GetIndex(), ObjectiveSubTypes), false);
				}
				++It;
			}
			break;
		}
		default: case EObjectiveSubTypes::None: NMT_CHECKF(0);
	}
}

void UQuestManager::AddQuestChangedStatic(const UObject* Object, const FOnQuestChangedStatic& Delegate)
{
	AddQuestChanged(FQuestChangedItem(Object, Delegate));
}

void UQuestManager::AddQuestChangedDynamic(const UObject* Object, const FOnQuestChangedDynamic& Delegate)
{
	AddQuestChanged(FQuestChangedItem(Object,  Delegate));
}

void UQuestManager::BroadCastQuestChangedEvent(const EObjectiveTypes ObjectiveTypes, const FObjectiveSubTypeContext ObjectiveSubTypeContext, const bool IsQuestCompleted)
{
	for(FQuestChangedItem Item : Subscribers)
	{
		if(Item.Subscribe.IsBound())
			Item.Subscribe.Execute(ObjectiveTypes, ObjectiveSubTypeContext, IsQuestCompleted);
	}
}

void UQuestManager::AddQuestChanged(const FQuestChangedItem& QuestChangedItem)
{
	if(Subscribers.Contains(QuestChangedItem)) return;
	Subscribers.Push(QuestChangedItem);
}
