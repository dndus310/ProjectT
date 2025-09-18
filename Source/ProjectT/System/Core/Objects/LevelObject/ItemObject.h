// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreInteractionObject.h"
#include "ProjectT/Data/Gen/GenerateEnumItemTypes.h"
#include "ProjectT/Data/Gen/GenerateStructItem.h"
#include "ProjectT/System/Core/Interfaces/ItemObjectControl.h"
#include "ItemObject.generated.h"

UCLASS()
class PROJECTT_API AItemObject : public ACoreInteractionObject, public IItemObjectControl
{
	GENERATED_BODY()

public:
	AItemObject();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback) override;
	virtual void SuccessCompleteAction_Implementation() override;
	
	virtual void SetVisibility(bool InbVisible) override;
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;	
	virtual void ObtainItem_Implementation()override;
	virtual FName GetItemRowName_Implementation()override;

	UFUNCTION(BlueprintCallable)
	void SetItem(const FName& ItemRowName);

	UFUNCTION(BlueprintNativeEvent)
	void ItemIdle(EItemTypes TargetItem);

	UFUNCTION(BlueprintNativeEvent)
	void ItemFlyUP();

	UFUNCTION(BlueprintNativeEvent)
	void ItemFlyToUI();

	UFUNCTION(BlueprintPure, Category = "ItemObject")
	EItemTypes GetItemType();

	FItem GetItemData();
	void BeginStateItem();	
	void SetItemByWeightFromTable(int32 GroupID);

protected:
	UFUNCTION(BlueprintCallable)
	virtual void UseItem();

private:
#if WITH_EDITOR

	UFUNCTION()
	void ItemSetOnlyEditor();

	UFUNCTION()
	void DrawItemDebugText();

	UFUNCTION()
	void SetItemMeshEditor();
#endif

public:
	UPROPERTY(BlueprintReadWrite)
	FVector ItemInitLocation;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	FItem ItemData;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	EItemTypes ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> DefaultStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> DefaultDynamicMeshComponent;

private:
	UPROPERTY()
	int32 SavedItemLabel;

	UPROPERTY()
	UDataTable* ItemSetDT;

	UPROPERTY()
	uint8 bObtain : 1;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UTextRenderComponent> ItemDebugTEXT;	

	UPROPERTY()
	TArray<FItem> ItemDataForEditor;
#endif

};
