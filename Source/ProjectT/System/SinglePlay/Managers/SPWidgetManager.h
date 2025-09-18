// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProjectT/Data/Gen/GenerateStructItem.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveTypes.h"
#include "ProjectT/System/Core/Components/PlayerInventoryComponent.h"
#include "ProjectT/System/Core/Managers/CoreWidgetManager.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"
#include "ProjectT/System/SinglePlay/Widget/SPCommonHUD.h"
#include "SPWidgetManager.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnStartGame);
DECLARE_DYNAMIC_DELEGATE(FOnPlayerDead);
DECLARE_DYNAMIC_DELEGATE(FOnGameDefeat);
DECLARE_DYNAMIC_DELEGATE(FOnGameVictory);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInteractionIndicator, const bool, bEnabled);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdatePlayerHP, const float, HPRatio);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdatePlayerStamina, const float, StaminaRatio);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdateTime, const int32, RemainTime);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnUpdateIndicator, const FString&, InteractionText, const bool, bVisibility);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnUpdateSlotFocus, const int&, SlotIndex, const bool, bFocused);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnQuestUpdate, const EObjectiveTypes, ObjectiveTypes, const FObjectiveSubTypeContext, ObjectiveSubTypeContext, const bool, IsQuestCompleted);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnSkillUpdate, TSubclassOf<class ACoreSkill>, SkillClass, const float, RemainTimeRatio, const bool, bCoolDownComplete);

DECLARE_DYNAMIC_DELEGATE_FourParams(FOnUpdateConsumableSlot, const FName&, RowName, const FItem&, ItemData, const int32, SlotIndex, const int32, ResultCount);
DECLARE_DYNAMIC_DELEGATE_FourParams(FOnUpdateGimmickSlot, const FName&, RowName, const FPlayerGimmickSlotData&, ItemData, const int32, SlotIndex, const int32, ResultCount);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnQuestInitialized, const TArray<struct FObjective>&, QuestList);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSkillInitialized, TSubclassOf<class ACoreSkill>, SkillClass, const FString&, IconPath);

UCLASS()
class PROJECTT_API USPWidgetManager : public UCoreWidgetManager
{
	GENERATED_BODY()
	
public:
	USPWidgetManager();
	
	virtual void InitializeManager(const UObject* WorldObjectContext) override;
	virtual void ReleaseManager() override;
	void ExecuteOnGameDefeat();
	void ExecuteOnGameVictory();

	UFUNCTION()
	void ExecuteStartGame();
	
	UFUNCTION()
	void ExecuteOnQuestInitialized(const TArray<struct FObjective>& QuestList);

	UFUNCTION()
	void ExecuteOnQuestUpdate(const EObjectiveTypes ObjectiveTypes, const FObjectiveSubTypeContext ObjectiveSubTypeContext, const bool IsQuestCompleted);

	UFUNCTION()
	void ExecuteOnPlayerDead();
	
	UFUNCTION()
	void ExecuteOnInteractionIndicator(const bool bEnabled);
	
	UFUNCTION()
	void ExecuteOnUpdatePlayerHP(const float HPRatio);
	
	UFUNCTION()
	void ExecuteOnUpdatePlayerStamina(const float StaminaRatio);
	
	UFUNCTION()
	void ExecuteOnUpdateTime(const int32 RemainTime);
	
	UFUNCTION()
	void ExecuteOnUpdateIndicator(const FString& InteractionText, const bool bVisibility);
	
	UFUNCTION()
	void ExecuteOnUpdateSlotFocus(const int& SlotIndex, const bool bFocused);
	
	UFUNCTION()
	void ExecuteOnSkillInitialized(TSubclassOf<class ACoreSkill> SkillClass, const FString& IconPath);

	UFUNCTION()
	void ExecuteOnSkillUpdate(TSubclassOf<class ACoreSkill> SkillClass, const float RemainTimeRatio, const bool bCoolDownComplete);
	
	UFUNCTION()
	void ExecuteOnUpdateConsumableSlot(const FName& RowName, const FItem& ItemData, const int32 SlotIndex, const int32 ResultCount);
	
	UFUNCTION()
	void ExecuteOnUpdateGimmickSlot(const FName& RowName, const FPlayerGimmickSlotData& ItemData, const int32 SlotIndex, const int32 ResultCount);

private:
	friend USPCommonHUD;
	
	UPROPERTY()
	FOnPlayerDead OnPlayerDead;

	UPROPERTY()
	FOnGameDefeat OnGameDefeat;

	UPROPERTY()
	FOnGameVictory OnGameVictory;

	UPROPERTY()
	FOnInteractionIndicator OnInteractionIndicator;

	UPROPERTY()
	FOnUpdatePlayerHP OnUpdatePlayerHP;

	UPROPERTY()
	FOnUpdatePlayerStamina OnUpdatePlayerStamina;

	UPROPERTY()
	FOnUpdateTime OnUpdateTime;

	UPROPERTY()
	FOnUpdateIndicator OnUpdateIndicator;

	UPROPERTY()
	FOnUpdateSlotFocus OnUpdateSlotFocus;

	UPROPERTY()
	FOnQuestUpdate OnQuestUpdate;

	UPROPERTY()
	FOnUpdateConsumableSlot OnUpdateConsumableSlot;

	UPROPERTY()
	FOnUpdateGimmickSlot OnUpdateGimmickSlot;

	UPROPERTY()
	FOnSkillUpdate OnSkillUpdate;

	UPROPERTY()
	FOnQuestInitialized OnQuestInitialized;
	
	UPROPERTY()
	FOnStartGame OnStartGame;

	UPROPERTY()
	FOnSkillInitialized OnSkillInitialized;
};
