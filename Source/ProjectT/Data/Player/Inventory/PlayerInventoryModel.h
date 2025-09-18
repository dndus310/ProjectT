// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/Data/Gen/GenerateStructItem.h"
#include "UObject/Object.h"
#include "PlayerInventoryModel.generated.h"

USTRUCT(Blueprintable)
struct FPlayerGimmickSlotData
{
	GENERATED_BODY()
	FPlayerGimmickSlotData() :
		ItemRowName("None"),
		ItemCount(0)
	{
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FName ItemRowName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FItem ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 ItemCount;
};

USTRUCT(Blueprintable)
struct FPlayerSpendSlotData
{
	GENERATED_BODY()
	FPlayerSpendSlotData() :
		ItemRowName("None")
	{
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FName ItemRowName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FItem ItemData;
};

UCLASS()
class PROJECTT_API UPlayerInventoryModel : public UObject
{
	GENERATED_BODY()
};
