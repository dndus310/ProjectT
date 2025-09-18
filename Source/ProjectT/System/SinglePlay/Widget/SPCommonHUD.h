// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Widget/CommonHUD.h"
#include "SPCommonHUD.generated.h"

enum class EObjectiveTypes: uint8;

UCLASS()
class PROJECTT_API USPCommonHUD : public UCommonHUD
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void InitializeBindEvent() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void ReceiveStartGame();

	UFUNCTION(BlueprintNativeEvent)
	void ReceivePlayerDead();

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveGameDefeat();

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveGameVictory();

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveInteractionIndicator(const bool bEnabled);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveUpdatePlayerHP(const float HPRatio);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveUpdatePlayerStamina(const float StaminaRatio);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveUpdateTime(const int32 RemainTime);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveUpdateIndicator(const FString& InteractionText, const bool bVisibility);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveUpdateSlotFocus(const int& SlotIndex, const bool bFocused);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveQuestUpdate(const EObjectiveTypes ObjectiveTypes, const struct FObjectiveSubTypeContext ObjectiveSubTypeContext, const bool IsQuestCompleted);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveSkillUpdate(TSubclassOf<class ACoreSkill> SkillClass, const float RemainTimeRatio, const bool bCoolDownComplete);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveUpdateConsumableSlot(const FName& RowName, const FItem& ItemData, const int32 SlotIndex, const int32 ResultCount);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveUpdateGimmickSlot(const FName& RowName, const FPlayerGimmickSlotData& ItemData, const int32 SlotIndex, const int32 ResultCount);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveQuestInitialized(const TArray<struct FObjective>& QuestList);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveSkillInitialized(TSubclassOf<class ACoreSkill> SkillClass, const FString& IconPath);
};
