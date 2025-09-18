// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"

#include "EnvironmentQuery/EnvQueryDebugHelpers.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/Interfaces/WorldObjectProvider.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"
#include "ProjectT/System/Core/Objects/LevelObject/CoreInteractionObject.h"
#include "ProjectT/System/Core/Objects/LevelObject/ItemObject.h"

class AWorldGameMode;

UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	//fixme initialize from character inven data ex.slot num
	for (int i = 0; i < 3; i++)
	{
		GimmickSlot.Add(FPlayerGimmickSlotData());
	}
	for (int i = 0; i < 3; i++)
	{
		SpendSlot.Add(FPlayerSpendSlotData());
	}
}

void UPlayerInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPlayerInventoryComponent::TryAddData(AActor* InGetObject)
{
	bool LocbRet = false;
	if(!InGetObject->GetClass()->IsChildOf(AItemObject::StaticClass())) return false;
	AItemObject* LocItemObject = Cast<AItemObject>(InGetObject);
	switch(IObjectInteractionControl::Execute_GetInteractionObjectType(InGetObject))
	{
		case EInteractionObjectType::Item:
		{
			switch (LocItemObject->GetItemType())
			{
				case EItemTypes::Gimick:
				{
					LocbRet = RefreshGimmickSlotData(LocItemObject);
					break;
				}
				case EItemTypes::Usable:
				{
					LocbRet =  RefreshSpendSlotData(LocItemObject);
					break;
				}
				case EItemTypes::Collection: LocbRet = true; break;
				default: case EItemTypes::None: NMT_CHECKF(0);
			}
			break;
		}
		case EInteractionObjectType::LevelObject: NOT_IMPLEMENTED(); break;
		default: case EInteractionObjectType::None: NMT_CHECKF(0);
	}
	return LocbRet;
}

bool UPlayerInventoryComponent::RefreshGimmickSlotData(AItemObject* InGetItem)
{
	bool LocbValidSlot = false;
	bool bAddedSuccess = false;
	int LocValidSlotIndex = -1;
	for (int i = 0; i < GimmickSlot.Num(); i++)
	{
		if(GimmickSlot[i].ItemRowName == InGetItem->GetObjectRowName())
		{
			LocValidSlotIndex = i;
			LocbValidSlot = true;
			break;
		}
	}

	if(LocbValidSlot)
	{
		GimmickSlot[LocValidSlotIndex].ItemCount ++;
		if (UpdateGimmickInventoryData.IsBound()) UpdateGimmickInventoryData.Broadcast(InGetItem->GetObjectRowName(), GimmickSlot[LocValidSlotIndex], LocValidSlotIndex, 1);
		bAddedSuccess = true;
	}
	else
	{
		for (int i = 0; i < GimmickSlot.Num(); i++)
		{
			if (GimmickSlot[i].ItemCount == 0)
			{
				GimmickSlot[i].ItemRowName = InGetItem->GetObjectRowName();
				GimmickSlot[i].ItemData = InGetItem->GetItemData();
				GimmickSlot[i].ItemCount ++;
				if (UpdateGimmickInventoryData.IsBound()) UpdateGimmickInventoryData.Broadcast(InGetItem->GetObjectRowName(), GimmickSlot[i], i, 1);
				bAddedSuccess = true;
				break;
			}
		}
	}
	return bAddedSuccess;
}

bool UPlayerInventoryComponent::RefreshSpendSlotData(AItemObject* InGetItem)
{
	bool LocbValidSlot = false;
	int LocValidSlotIndex = -1;
	for (int i = 0; i < SpendSlot.Num(); i++)
	{
		if(SpendSlot[i].ItemRowName == TEXT("None"))
		{
			LocValidSlotIndex = i;
			LocbValidSlot = true;
			break;
		}
	}
	if(LocbValidSlot)
	{
		SpendSlot[LocValidSlotIndex].ItemRowName = InGetItem->GetObjectRowName();
		SpendSlot[LocValidSlotIndex].ItemData = InGetItem->GetItemData();
		if(UpdateSpendInventoryData.IsBound()) UpdateSpendInventoryData.Broadcast(InGetItem->GetObjectRowName(), InGetItem->GetItemData(), LocValidSlotIndex, 1);
	}
	else
	{
		NMT_LOG("Spend Slot Full Empty Slot")
	}
	return LocbValidSlot;
}

void UPlayerInventoryComponent::UseGimmickSlotItem(int InSlotNumber)
{
	GimmickSlot[InSlotNumber].ItemCount--;
	if (UpdateGimmickInventoryData.IsBound()) UpdateGimmickInventoryData.Broadcast(GimmickSlot[InSlotNumber].ItemRowName, GimmickSlot[InSlotNumber], InSlotNumber, -1);
	if (GimmickSlot[InSlotNumber].ItemCount == 0) GimmickSlot[InSlotNumber] = FPlayerGimmickSlotData();
}

void UPlayerInventoryComponent::UseSpendSlotItem(int InSlotNumber, int& OutSkillId)
{
	if (SpendSlot[InSlotNumber].ItemRowName == TEXT("None")) OutSkillId = -1;
	else OutSkillId = SpendSlot[InSlotNumber].ItemData.iItemSkillId;
}

void UPlayerInventoryComponent::EmptySpendSlotItem(int InSlotNumber)
{
	SpendSlot[InSlotNumber] = FPlayerSpendSlotData();
	if (UpdateSpendInventoryData.IsBound()) UpdateSpendInventoryData.Broadcast("None", FItem(), InSlotNumber, -1);
}

void UPlayerInventoryComponent::GetReserveItemRowNames(TArray<FName>& OutReserveRowNames)
{
	OutReserveRowNames.Empty();
	TArray<FName> LocReserveRowNames;
	for(FPlayerGimmickSlotData LocCurItem : GimmickSlot)
	{
		if(LocCurItem.ItemRowName != "None")
			LocReserveRowNames.Add(LocCurItem.ItemRowName);
	}
	OutReserveRowNames = LocReserveRowNames;	
}

void UPlayerInventoryComponent::GetMatchSlotNumber(FName InRowName, int& OutSlotNumber)
{
	int LocIndex = -1;
	for(int i = 0; i < GimmickSlot.Num(); i++)
	{
		if(GimmickSlot[i].ItemRowName == InRowName)
		{
			LocIndex = i;
			break;
		}
	}
	OutSlotNumber = LocIndex;
}

