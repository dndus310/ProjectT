// Fill out your copyright notice in the Description page of Project Settings.


#include "SPCommonHUD.h"

#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"
#include "ProjectT/System/SinglePlay/GameModes/SPGameModeBase.h"
#include "ProjectT/System/SinglePlay/Managers/SPWidgetManager.h"
#include "ProjectT/System/SinglePlay/Interfaces/SPObjectProvider.h"

void USPCommonHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void USPCommonHUD::InitializeBindEvent()
{
	Super::InitializeBindEvent();
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	ASPGameModeBase* SPGM = UGlobalUtilsLibrary::GetGameModeChecked<ASPGameModeBase>(World);
	USPWidgetManager* SPWM = ISPObjectProvider::Execute_GetSPWidgetManager(SPGM);
	SPWM->OnGameDefeat.BindDynamic(this, &USPCommonHUD::ReceiveGameDefeat);
	SPWM->OnGameVictory.BindDynamic(this, &USPCommonHUD::ReceiveGameVictory);
	SPWM->OnInteractionIndicator.BindDynamic(this, &USPCommonHUD::ReceiveInteractionIndicator);
	SPWM->OnPlayerDead.BindDynamic(this, &USPCommonHUD::ReceivePlayerDead);
	SPWM->OnQuestInitialized.BindDynamic(this, &USPCommonHUD::ReceiveQuestInitialized);
	SPWM->OnQuestUpdate.BindDynamic(this, &USPCommonHUD::ReceiveQuestUpdate);
	SPWM->OnSkillInitialized.BindDynamic(this, &USPCommonHUD::ReceiveSkillInitialized);
	SPWM->OnSkillUpdate.BindDynamic(this, &USPCommonHUD::ReceiveSkillUpdate);
	SPWM->OnUpdateIndicator.BindDynamic(this, &USPCommonHUD::ReceiveUpdateIndicator);
	SPWM->OnUpdateTime.BindDynamic(this, &USPCommonHUD::ReceiveUpdateTime);
	SPWM->OnUpdateConsumableSlot.BindDynamic(this, &USPCommonHUD::ReceiveUpdateConsumableSlot);
	SPWM->OnUpdateGimmickSlot.BindDynamic(this, &USPCommonHUD::ReceiveUpdateGimmickSlot);
	SPWM->OnUpdatePlayerStamina.BindDynamic(this, &USPCommonHUD::ReceiveUpdatePlayerStamina);
	SPWM->OnUpdateSlotFocus.BindDynamic(this, &USPCommonHUD::ReceiveUpdateSlotFocus);
	SPWM->OnUpdatePlayerHP.BindDynamic(this, &USPCommonHUD::ReceiveUpdatePlayerHP);
	SPWM->OnStartGame.BindDynamic(this, &USPCommonHUD::ReceiveStartGame);
}

void USPCommonHUD::ReceiveStartGame_Implementation()
{
}

void USPCommonHUD::ReceivePlayerDead_Implementation()
{
}

void USPCommonHUD::ReceiveGameDefeat_Implementation()
{
}

void USPCommonHUD::ReceiveGameVictory_Implementation()
{
}

void USPCommonHUD::ReceiveInteractionIndicator_Implementation(const bool bEnabled)
{
}

void USPCommonHUD::ReceiveUpdatePlayerHP_Implementation(const float HPRatio)
{
}

void USPCommonHUD::ReceiveUpdatePlayerStamina_Implementation(const float StaminaRatio)
{
}

void USPCommonHUD::ReceiveUpdateTime_Implementation(const int32 RemainTime)
{
}

void USPCommonHUD::ReceiveUpdateIndicator_Implementation(const FString& InteractionText, const bool bVisibility)
{
}

void USPCommonHUD::ReceiveUpdateSlotFocus_Implementation(const int& SlotIndex, const bool bFocused)
{
}

void USPCommonHUD::ReceiveQuestUpdate_Implementation(const EObjectiveTypes ObjectiveTypes, const struct FObjectiveSubTypeContext ObjectiveSubTypeContext, const bool IsQuestCompleted)
{
}

void USPCommonHUD::ReceiveSkillUpdate_Implementation(TSubclassOf<class ACoreSkill> SkillClass, const float RemainTimeRatio, const bool bCoolDownComplete)
{
}

void USPCommonHUD::ReceiveUpdateConsumableSlot_Implementation(const FName& RowName, const FItem& ItemData, const int32 SlotIndex, const int32 ResultCount)
{
}

void USPCommonHUD::ReceiveUpdateGimmickSlot_Implementation(const FName& RowName, const FPlayerGimmickSlotData& ItemData, const int32 SlotIndex, const int32 ResultCount)
{
}

void USPCommonHUD::ReceiveQuestInitialized_Implementation(const TArray<struct FObjective>& QuestList)
{
}

void USPCommonHUD::ReceiveSkillInitialized_Implementation(TSubclassOf<class ACoreSkill> SkillClass, const FString& IconPath)
{
}
