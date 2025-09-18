// Fill out your copyright notice in the Description page of Project Settings.


#include "SPEnemyVisionManager.h"

#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"

USPEnemyVisionManager::USPEnemyVisionManager() :
	OuterObject(nullptr)
{
}

void USPEnemyVisionManager::InitializeManager(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);	
}

void USPEnemyVisionManager::ReleaseManager()
{
	OuterObject = nullptr;
	FocusedNpc = nullptr;
	DetectNpcList.Empty();
}

void USPEnemyVisionManager::RegisterFocused(class ACoreNpc* InDetectNpc)
{
	if(FocusedNpc != nullptr)
	{
		if(FocusedNpc == InDetectNpc)
		{
			if(DetectNpcList.Contains(InDetectNpc))
			{
				UnregisterDetection(InDetectNpc);
			}
			else
			{
				FocusedNpc->SetVisionMode(EVisionMode::None);
			}
			
			FocusedNpc = nullptr;
		}
		else
		{
			FocusedNpc->SetVisionMode(EVisionMode::None);
			FocusedNpc = InDetectNpc;
			FocusedNpc->SetVisionMode(EVisionMode::Cone);
		}
	}
	else
	{
		if(DetectNpcList.Contains(InDetectNpc))
		{
			UnregisterDetection(InDetectNpc);
		}
		else
		{
			UnregisterAllDetection();
			FocusedNpc = InDetectNpc;
			FocusedNpc->SetVisionMode(EVisionMode::Cone);
		}
	}
}

void USPEnemyVisionManager::RegisterDetection(class ACoreNpc* InDetectNpc)
{
	if(DetectNpcList.Contains(InDetectNpc))
		return;

	if(FocusedNpc != nullptr)
	{
		FocusedNpc->SetVisionMode(EVisionMode::None);
		FocusedNpc = nullptr;
	}
	
	DetectNpcList.Add(InDetectNpc);
	InDetectNpc->SetVisionMode(EVisionMode::Cone);
	// UpdateSightState();
}

void USPEnemyVisionManager::UnregisterDetection(class ACoreNpc* InDetectNpc)
{
	if(!DetectNpcList.Contains(InDetectNpc))
		return;
	
	DetectNpcList.Remove(InDetectNpc);
	InDetectNpc->SetVisionMode(EVisionMode::None);
	// UpdateSightState();
}

void USPEnemyVisionManager::UnregisterFocused()
{
	if(FocusedNpc != nullptr)
	{
		FocusedNpc->SetVisionMode(EVisionMode::None);
		FocusedNpc = nullptr;
	}	
}

void USPEnemyVisionManager::UnregisterAllDetection()
{
	for(ACoreNpc* Npc : DetectNpcList)
	{
		Npc->SetVisionMode(EVisionMode::None);
	}

	DetectNpcList.Empty();
}

bool USPEnemyVisionManager::IsFocusedNpc(class ACoreNpc* InNpc) const
{
	return FocusedNpc && FocusedNpc == InNpc;
}

void USPEnemyVisionManager::UpdateSightState()
{
	for(int i = 0; i < DetectNpcList.Num(); i++)
	{
		if(i == 0)
		{
			DetectNpcList[i]->SetVisionMode(EVisionMode::Cone);
		}
		else
		{
			DetectNpcList[i]->SetVisionMode(EVisionMode::Line);
		}
	}
}
