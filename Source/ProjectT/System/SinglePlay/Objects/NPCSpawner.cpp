// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"

ANPCSpawner::ANPCSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName("RootSceneComponent"));
	NMT_CHECKF(RootSceneComponent);
	
	SetRootComponent(RootSceneComponent);
}

void ANPCSpawner::Destroyed()
{
#if WITH_EDITOR
	if(IsValid(SpawnedNPCActor))
	{
		SpawnedNPCActor->Destroy();
		SpawnedNPCActor = nullptr;
	}
#endif
	
	Super::Destroyed();
}

#if WITH_EDITOR
void ANPCSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(!GEditor) return;	
	SpawnNPCByTable();
}

void ANPCSpawner::SpawnNPCByTable()
{
	const FString LabelRowName = *GetActorLabel();
	if(LabelRowName.IsNumeric())
	{

		UWorld* World = GetWorld();
		if(IsValid(World))
		{
			// @FIXME : 테이블에 따른 정보 필요

			if(IsValid(SpawnedNPCActor))
			{
				SpawnedNPCActor->Destroy();
				SpawnedNPCActor = nullptr;
			}

			// @FIXME : 임시 클래스
			FString Path = TEXT("/Game/Characters/NPC/Agent/Blueprints/BP_SPNpcPatrol.BP_SPNpcPatrol_C");
			if (UClass* TargetClass = LoadObject<UClass>(nullptr, *Path))
			{
				SpawnedNPCActor = World->SpawnActorDeferred<AActor>(TargetClass, FTransform::Identity,nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::MultiplyWithRoot);
				if(SpawnedNPCActor)
				{
					SpawnedNPCActor->SetActorLabel(LabelRowName);
					UGameplayStatics::FinishSpawningActor(SpawnedNPCActor, FTransform::Identity);					
					SpawnedNPCActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					NMT_LOG("NPC spawn successed");
				}
			}
		}
		else
		{
			NMT_LOG("NPC spawn failed. World is not valid");
		}
	}
}
#endif