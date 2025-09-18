// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemObject.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/Data/Gen/GenerateStructItemSet.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateStructMeshList.h"

#if WITH_EDITOR
#include "Components/TextRenderComponent.h"
#include "ProjectT/Data/Gen/GenerateStructLocalization.h"
#endif



AItemObject::AItemObject()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionObjectType = EInteractionObjectType::Item;

	DefaultStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultStaticMeshComponent"));
	DefaultStaticMeshComponent->SetupAttachment(MeshSceneComponent);
	DefaultDynamicMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DefaultDynamicMeshComponent"));
	DefaultDynamicMeshComponent->SetupAttachment(MeshSceneComponent);
	
#if WITH_EDITOR
	if(!GEditor) return;
	ItemDebugTEXT = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("DEBUG"));
	ItemDebugTEXT->SetupAttachment(MeshSceneComponent);
	ItemSetDT = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::ItemSet);
	ItemSetDT->OnDataTableChanged().AddUObject(this, &AItemObject::ItemSetOnlyEditor);
#endif
}

void AItemObject::BeginPlay()
{
	Super::BeginPlay();

	if(NMT_ENSURE(SavedItemLabel > 0))
		SetItemByWeightFromTable(SavedItemLabel);

#if WITH_EDITOR
	if(!GEditor) return;
	ItemDebugTEXT->DestroyComponent();
	ItemDataForEditor.Empty();
#endif
}

void AItemObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemObject::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if(!GEditor) return;
	const FString LabelRowName = *GetActorLabel();
	if(LabelRowName.IsNumeric())
	{
		SavedItemLabel = FCString::Atoi(*LabelRowName);
		ItemSetOnlyEditor();
	}
#endif
}

void AItemObject::TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback)
{
	Super::TryInteraction(InSendData, Callback);
	Callback.Execute(EInteractionResultType::SuccessComplete);
}

void AItemObject::SuccessCompleteAction_Implementation()
{
	Super::SuccessCompleteAction_Implementation();
}

void AItemObject::SetVisibility(bool InbVisible)
{
	Super::SetVisibility(InbVisible);
	DefaultStaticMeshComponent->SetRenderCustomDepth(InbVisible);
	DefaultDynamicMeshComponent->SetRenderCustomDepth(InbVisible);
}

void AItemObject::ShowVisual_Implementation()
{
	Super::ShowVisual_Implementation();
}

void AItemObject::HideVisual_Implementation()
{
	Super::HideVisual_Implementation();
}

void AItemObject::ItemIdle_Implementation(EItemTypes TargetItem)
{
}

void AItemObject::ItemFlyUP_Implementation()
{
}

void AItemObject::ItemFlyToUI_Implementation()
{
}

FItem AItemObject::GetItemData()
{
	return ItemData;
}

void AItemObject::BeginStateItem()
{
	ItemInitLocation = GetActorLocation();
	switch(ItemType)
	{
	case EItemTypes::None:
	{
		break;
	}
	case EItemTypes::Gimick:
	{
		ItemIdle(EItemTypes::Gimick);
		break;
	}
	case EItemTypes::Usable:
	{
		ItemIdle(EItemTypes::Usable);
		break;
	}
	case EItemTypes::Collection:
	{
		ItemIdle(EItemTypes::Collection);
		break;
	}
	default:
		break;
	}
}

EItemTypes AItemObject::GetItemType()
{
	return ItemType;
}

void AItemObject::SetItem(const FName& ItemRowName)
{
	if(!ObjectRowName.IsNone()) return;
	
	UDataTable* ItemDT = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Item);
	FItem* FindItem = ItemDT->FindRow<FItem>(ItemRowName, FString());

	if(!FindItem) NMT_MSG_CHECKF(0, "Can't find Item ID %s from DT_ItemSet!", *ItemRowName.ToString());

	ItemData = *FindItem;
	ObjectRowName = ItemRowName;

	UDataTable* MLSTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::MeshList);
	FMeshList* ItemMeshData = MLSTable->FindRow<FMeshList>(FROM_INT(ItemData.iItemMeshId), TEXT("FMeshList"));
	if(!ItemMeshData) return;

	if(ItemMeshData->sMeshPath.Contains(FString("StaticMesh")))
	{
		UStaticMesh* ItemStaticMesh = UAssetUtilsLibrary::GetStaticMeshFromPath(ItemMeshData->sMeshPath);
		if(!ItemStaticMesh) return;
		DefaultDynamicMeshComponent->SetSkeletalMesh(nullptr);
		DefaultStaticMeshComponent->SetStaticMesh(ItemStaticMesh);
	}

	else if(ItemMeshData->sMeshPath.Contains(FString("SkeletalMesh")))
	{
		USkeletalMesh* ItemDynamicMesh = UAssetUtilsLibrary::GetSkeletalMeshFromPath(ItemMeshData->sMeshPath);
		if(!ItemDynamicMesh) return;
		DefaultStaticMeshComponent->SetStaticMesh(nullptr);
		DefaultDynamicMeshComponent->SetSkeletalMesh(ItemDynamicMesh);
	}

	UEnum* EnumItemType = StaticEnum<EItemTypes>();
	if(!EnumItemType) return;
	int32 EnumIDX = EnumItemType->GetValueByName(*ItemData.sItemType);
	if(EnumIDX == INDEX_NONE) return;
	ItemType = static_cast<EItemTypes>(EnumIDX);

	BeginStateItem();
}

void AItemObject::SetItemByWeightFromTable(int32 GroupID)
{
	TArray<FItemSet*> ItemSetList;
	TArray<int32> FindItemIDs;
	ItemSetDT = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::ItemSet);
	ItemSetDT->GetAllRows<FItemSet>(TEXT(""), ItemSetList);

	for(auto FindSet : ItemSetList)
	{
		if(FindSet->iItemGroupId == GroupID)
		{
			for(int i = 1; i <= FindSet->iProbWeight; i++)
			{
				FindItemIDs.Add(FindSet->iItemId);
			}
		}
	}

	if(FindItemIDs.Num() == 0) return;

	UGlobalUtilsLibrary::ShuffleArray(FindItemIDs);
	const FName DecidedRowName = FName(FString::FromInt(FindItemIDs[0]));
	SetItem(DecidedRowName);
}

void AItemObject::ObtainItem_Implementation()
{
	if(!bObtain)
	{
		bObtain = true;
		SetActorEnableCollision(false);
		switch(ItemType)
		{
		case EItemTypes::None:
		{
			break;
		}
		case EItemTypes::Gimick:
		{
			ItemFlyUP();
			break;
		}
		case EItemTypes::Usable:
		{
			ItemFlyUP();
			break;
		}
		case EItemTypes::Collection:
		{
			ItemFlyToUI();
			break;
		}
		default:
			break;
		}
	}
}

FName AItemObject::GetItemRowName_Implementation()
{
	return GetObjectRowName();
}

void AItemObject::UseItem()
{
	NOT_IMPLEMENTED()
}

#if WITH_EDITOR
void AItemObject::ItemSetOnlyEditor()
{
	ItemDataForEditor.Empty();
	DrawItemDebugText();
	SetItemMeshEditor();
}
void AItemObject::DrawItemDebugText()
{
	ItemDebugTEXT->SetTextRenderColor(FColor(0, 255, 0));

	if(SavedItemLabel != 0)
	{
		ItemDebugTEXT->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);

		TArray<FItemSet*> ItemSetList;
		TSet<int32> FindItemIDs;
		TMap<int32, int32> LocItemData;
		int32 SumWeight = 0;

		ItemSetDT->GetAllRows<FItemSet>(TEXT(""), ItemSetList);
		LocItemData.Empty();
		for(FItemSet* FindSet : ItemSetList)
		{
			if(FindSet->iItemGroupId == SavedItemLabel)
			{
				LocItemData.Add(FindSet->iItemId, FindSet->iProbWeight);
				SumWeight += FindSet->iProbWeight;
				FindItemIDs.Add(FindSet->iItemId);
			}
		}

		if(FindItemIDs.Num() == 0)
		{
			ItemDebugTEXT->SetTextRenderColor(FColor(255, 0, 0));
			ItemDebugTEXT->SetText(FText::FromString("NONE Data"));
			return;
		}

		if(SumWeight <= 0)
		{
			ItemDebugTEXT->SetTextRenderColor(FColor(255, 0, 0));
			ItemDebugTEXT->SetText(FText::FromString("Weight Is Zero : NeverSetItem"));
			return;
		}

		FString ItemDebugString = FString();
		UDataTable* ItemDT = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Item);

		for(int32 Item : FindItemIDs)
		{
			FLocalization LocString = {};
			FItem LocItem = {};
			float Probability = 0.f;
			FName DecidedItem = FName(FString::FromInt(Item));
			int32 LocItemRowName = FCString::Atoi(*DecidedItem.ToString());
			FItem* FindItem = ItemDT->FindRow<FItem>(DecidedItem, FString("Can't Find Item ID !!!!"));

			if(!FindItem)
			{
				ItemDebugTEXT->SetTextRenderColor(FColor(255, 0, 0));
				ItemDebugTEXT->SetText(FText::FromString("Can't Find ItemID"));
				return;
			}
			LocItem = *FindItem;
			ItemDataForEditor.AddUnique(LocItem);
			FName FindString = FName(FString::FromInt(LocItem.iItemNameId));
			UDataTable* StringDT = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::Localization);
			LocString = *(StringDT->FindRow<FLocalization>(FindString, FString()));

			Probability = static_cast<float>(*(LocItemData.Find(LocItemRowName))) / static_cast<float>(SumWeight);
			Probability *= 100;

			FString Result = FString::Printf(TEXT("%.2f"), Probability);

			ItemDebugString.Append(LocString.sEnglish);
			ItemDebugString.Append(":");
			ItemDebugString.Append(Result);
			ItemDebugString.Append("%");
			ItemDebugString.Append("\n");
		}
		ItemDebugTEXT->SetText(FText::FromString(ItemDebugString));
		ItemDebugTEXT->SetWorldLocation(GetActorLocation() + FVector(0, 0, 60));
	}
}
void AItemObject::SetItemMeshEditor()
{
	DefaultStaticMeshComponent->SetStaticMesh(nullptr);
	DefaultDynamicMeshComponent->SetSkeletalMesh(nullptr);

	if(ItemDataForEditor.Num() == 1)
	{
		FItem LocItem = ItemDataForEditor[0];

		UDataTable* MLSTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::MeshList);
		FMeshList* ItemMeshData = MLSTable->FindRow<FMeshList>(FROM_INT(LocItem.iItemMeshId), TEXT("FMeshList"));
		if(!ItemMeshData)
		{
			ItemDebugTEXT->SetTextRenderColor(FColor(255, 255, 0));
			return;
		}


		if(ItemMeshData->sMeshPath.Contains(FString("StaticMesh")))
		{
			UStaticMesh* ItemStaticMesh = UAssetUtilsLibrary::GetStaticMeshFromPath(ItemMeshData->sMeshPath);
			if(!ItemStaticMesh)
			{
				ItemDebugTEXT->SetTextRenderColor(FColor(255, 255, 0));
				return;
			}
			DefaultStaticMeshComponent->SetStaticMesh(ItemStaticMesh);
		}

		else if(ItemMeshData->sMeshPath.Contains(FString("SkeletalMesh")))
		{
			USkeletalMesh* ItemDynamicMesh = UAssetUtilsLibrary::GetSkeletalMeshFromPath(ItemMeshData->sMeshPath);
			if(!ItemDynamicMesh)
			{
				ItemDebugTEXT->SetTextRenderColor(FColor(255, 255, 0));
				return;
			}
			DefaultDynamicMeshComponent->SetSkeletalMesh(ItemDynamicMesh);
		}
	}
	else if(ItemDataForEditor.Num() == 0)
	{
		DefaultStaticMeshComponent->SetStaticMesh(nullptr);
		DefaultDynamicMeshComponent->SetSkeletalMesh(nullptr);
	}
	else
	{
		UStaticMesh* EditorMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Engine/EditorMeshes/EditorHelp.EditorHelp")));
		if(EditorMesh)
		{
			DefaultStaticMeshComponent->SetStaticMesh(EditorMesh);
		}
	}
}
#endif

