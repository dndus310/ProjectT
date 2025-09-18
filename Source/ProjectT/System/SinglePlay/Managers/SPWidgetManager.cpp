// Fill out your copyright notice in the Description page of Project Settings.


#include "SPWidgetManager.h"
#include "ProjectT/System/Core/Components/PlayerInventoryComponent.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"


USPWidgetManager::USPWidgetManager()
{
}

void USPWidgetManager::InitializeManager(const UObject* WorldObjectContext)
{
	Super::InitializeManager(WorldObjectContext);
}

void USPWidgetManager::ReleaseManager()
{
	OnPlayerDead.Unbind();
	OnGameDefeat.Unbind();
	OnGameVictory.Unbind();
	OnInteractionIndicator.Unbind();
	OnUpdatePlayerHP.Unbind();
	OnUpdatePlayerStamina.Unbind();
	OnUpdateTime.Unbind();
	OnUpdateIndicator.Unbind();
	OnUpdateSlotFocus.Unbind();
	OnQuestUpdate.Unbind();
	OnUpdateConsumableSlot.Unbind();
	OnUpdateGimmickSlot.Unbind();
	OnSkillUpdate.Unbind();
	OnQuestInitialized.Unbind();
	
	Super::ReleaseManager();
}

void USPWidgetManager::ExecuteOnGameDefeat()
{
	if(OnGameDefeat.IsBound())
		OnGameDefeat.Execute();
}

void USPWidgetManager::ExecuteOnGameVictory()
{
	if(OnGameVictory.IsBound())
		OnGameVictory.Execute();
}

void USPWidgetManager::ExecuteStartGame()
{
	if(OnStartGame.IsBound())
		OnStartGame.Execute();
}

void USPWidgetManager::ExecuteOnPlayerDead()
{
	if(!OnPlayerDead.IsBound()) return;
	OnPlayerDead.Execute();
}

void USPWidgetManager::ExecuteOnInteractionIndicator(const bool bEnabled)
{
	if(!OnInteractionIndicator.IsBound()) return;
	OnInteractionIndicator.Execute(bEnabled);
}

void USPWidgetManager::ExecuteOnUpdatePlayerHP(const float HPRatio)
{
	if(!OnUpdatePlayerHP.IsBound()) return;
	OnUpdatePlayerHP.Execute(HPRatio);
}

void USPWidgetManager::ExecuteOnUpdatePlayerStamina(const float StaminaRatio)
{
	if(!OnUpdatePlayerStamina.IsBound()) return;
	OnUpdatePlayerStamina.Execute(StaminaRatio);
}

void USPWidgetManager::ExecuteOnUpdateTime(const int32 RemainTime)
{
	if(!OnUpdateTime.IsBound()) return;
	OnUpdateTime.Execute(RemainTime);
}

void USPWidgetManager::ExecuteOnUpdateIndicator(const FString& InteractionText, const bool bVisibility)
{
	if(!OnUpdateIndicator.IsBound()) return;
	OnUpdateIndicator.Execute(InteractionText, bVisibility);
}

void USPWidgetManager::ExecuteOnUpdateSlotFocus(const int& SlotIndex, const bool bFocused)
{
	if(!OnUpdateSlotFocus.IsBound()) return;
	OnUpdateSlotFocus.Execute(SlotIndex, bFocused);
}

void USPWidgetManager::ExecuteOnSkillInitialized(TSubclassOf<class ACoreSkill> SkillClass, const FString& IconPath)
{
	if(!OnSkillInitialized.IsBound()) return;
	OnSkillInitialized.Execute(SkillClass, IconPath);
}

void USPWidgetManager::ExecuteOnQuestUpdate(const EObjectiveTypes ObjectiveTypes, const FObjectiveSubTypeContext ObjectiveSubTypeContext, const bool IsQuestCompleted)
{
	if(!OnQuestUpdate.IsBound()) return;
	OnQuestUpdate.Execute(ObjectiveTypes, ObjectiveSubTypeContext, IsQuestCompleted);
}

void USPWidgetManager::ExecuteOnSkillUpdate(TSubclassOf<class ACoreSkill> SkillClass, const float RemainTimeRatio, const bool bCoolDownComplete)
{
	if(!OnSkillUpdate.IsBound()) return;
	OnSkillUpdate.Execute(SkillClass, RemainTimeRatio, bCoolDownComplete);
}

void USPWidgetManager::ExecuteOnUpdateConsumableSlot(const FName& RowName, const FItem& ItemData, const int32 SlotIndex, const int32 ResultCount)
{
	if(!OnUpdateConsumableSlot.IsBound()) return;
	OnUpdateConsumableSlot.Execute(RowName, ItemData, SlotIndex, ResultCount);
}

void USPWidgetManager::ExecuteOnUpdateGimmickSlot(const FName& RowName, const FPlayerGimmickSlotData& ItemData, const int32 SlotIndex, const int32 ResultCount)
{
	if(!OnUpdateGimmickSlot.IsBound()) return;
	OnUpdateGimmickSlot.Execute(RowName, ItemData, SlotIndex, ResultCount);
}

void USPWidgetManager::ExecuteOnQuestInitialized(const TArray<struct FObjective>& QuestList)
{
	if(!OnQuestInitialized.IsBound()) return;
	OnQuestInitialized.Execute(QuestList);
}
