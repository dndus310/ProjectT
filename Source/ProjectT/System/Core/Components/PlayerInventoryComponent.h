// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectT/Data/Gen/GenerateStructItem.h"
#include "ProjectT/Data/Player/Inventory/PlayerInventoryModel.h"
#include "PlayerInventoryComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FUpdateGimmickInventory, const FName&, RowName, const FPlayerGimmickSlotData&, ItemData, const int32, SlotIndex, const int32, ResultCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FUpdateSpendInventory, const FName&, RowName, const FItem&, ItemData, const int32, SlotIndex, const int32, ResultCount);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTT_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInventoryComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UseSpendSlotItem(int InSlotNumber, int& OutSkillId);
	void EmptySpendSlotItem(int InSlotNumber);
	void GetReserveItemRowNames(TArray<FName>& OutReserveRowNames);
	void GetMatchSlotNumber(FName InRowName, int& OutSlotNumber);
	FORCEINLINE TArray<FPlayerGimmickSlotData> GetGimmickSlot() { return GimmickSlot; }

	UFUNCTION(BlueprintCallable)
	bool TryAddData(AActor* InGetObject);

	UFUNCTION(BlueprintCallable)
	void UseGimmickSlotItem(int InSlotNumber);
	
protected:
	bool RefreshGimmickSlotData(class AItemObject* InGetItem);
	bool RefreshSpendSlotData(AItemObject* InGetItem);	

public:
	UPROPERTY(BlueprintAssignable)
	FUpdateGimmickInventory UpdateGimmickInventoryData;

	UPROPERTY(BlueprintAssignable)
	FUpdateSpendInventory UpdateSpendInventoryData;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerGimmickSlotData> GimmickSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerSpendSlotData> SpendSlot;
	
	
};
