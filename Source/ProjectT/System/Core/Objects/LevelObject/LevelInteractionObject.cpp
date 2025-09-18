// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelInteractionObject.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "Components/BoxComponent.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "CeilObject.h"


ALevelInteractionObject::ALevelInteractionObject()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionObjectType = EInteractionObjectType::LevelObject;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComponent"));
	BoxCollision->SetupAttachment(MeshSceneComponent);
}

void ALevelInteractionObject::BeginPlay()
{
	Super::BeginPlay();	
}

void ALevelInteractionObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevelInteractionObject::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	const FString LabelRowName = *GetActorLabel();
	if (LabelRowName.IsNumeric()) ApplyObjectConfiguration(*LabelRowName);

	for(ACoreNpc* Npc : BindNPCs)
	{
		if(Npc)	Npc->SetActivatedOnStart(false);
	}
#endif
}

void ALevelInteractionObject::SetVisibility(bool InbVisible)
{
	Super::SetVisibility(InbVisible);
	TArray<USceneComponent*> ChildComponents;	
	GetRootComponent()->GetChildrenComponents(true, ChildComponents);

	for (USceneComponent* Child : ChildComponents)
	{
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Child))
		{
			PrimitiveComp->SetRenderCustomDepth(InbVisible);
		}
	}
}

void ALevelInteractionObject::ShowVisual_Implementation()
{
	Super::ShowVisual_Implementation();
}

void ALevelInteractionObject::HideVisual_Implementation()
{
	Super::HideVisual_Implementation();
}

void ALevelInteractionObject::Destroyed()
{
	Super::Destroyed();

#if WITH_EDITOR
	if(!BindNPCs.IsEmpty())
	{
		for(ACoreNpc* Npc : BindNPCs)
		{
			if(Npc)	Npc->SetActivatedOnStart(true); 
		}	
	}
#endif
}

#if WITH_EDITOR
void ALevelInteractionObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ALevelInteractionObject, BindNPCs))
	{
		if(!PrevSelectedNPCs.IsEmpty())
		{
			for(ACoreNpc* Npc : PrevSelectedNPCs)
			{
				if(Npc)	Npc->SetActivatedOnStart(true);									
			}
		}

		for(ACoreNpc* Npc : BindNPCs)
		{
			if(Npc)	Npc->SetActivatedOnStart(false); 
		}
		PrevSelectedNPCs = BindNPCs;
	}
}

void ALevelInteractionObject::ApplyObjectConfiguration(const FName& RowName)
{
	UDataTable* LevelObjTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::LevelObject);
	FLevelObject* ObjData = LevelObjTable->FindRow<FLevelObject>(RowName, TEXT("FLevelObjectList"));
	if (!ObjData)return;
	LevelObjectData = *ObjData;
}
#endif

void ALevelInteractionObject::ExecuteBindNPCEvent()
{
	for(ACoreNpc* Npc : BindNPCs)
	{
		if(Npc)
		{
			Npc->ActivateCharacter(true);
		}
	}
}

void ALevelInteractionObject::ExecuteBindCeilEvent()
{
	if(CeilObjects.Num() != 0)
	{
		for(ACeilObject* CeilObject : CeilObjects)
		{
			if(CeilObject)
			{
				CeilObject->ActivateCeil(true);
			}
		}		
	}
}
