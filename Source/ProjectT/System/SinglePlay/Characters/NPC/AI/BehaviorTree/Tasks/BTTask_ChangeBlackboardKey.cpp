// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeBlackboardKey.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "ProjectT/Data/Npc/NpcDataModel.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"

UBTTask_ChangeBlackboardKey::UBTTask_ChangeBlackboardKey() :
	BoolValue(false),
	VectorValue(FVector::ZeroVector),
	NpcStateEnumValue(ENpcState::Default),
	bIsBool(false),
	bIsVector(false),
	bIsNpcStateEnum(false)
{
	NodeName = TEXT("Change BlackboardKey");
}

#if WITH_EDITOR
void UBTTask_ChangeBlackboardKey::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(KeySelector.SelectedKeyType)
	{
		bIsBool = KeySelector.SelectedKeyType->IsChildOf(UBlackboardKeyType_Bool::StaticClass());
		bIsVector = KeySelector.SelectedKeyType->IsChildOf(UBlackboardKeyType_Vector::StaticClass());
		// FIXME
		bIsNpcStateEnum = KeySelector.SelectedKeyType->IsChildOf(UBlackboardKeyType_Enum::StaticClass());
	}
}
#endif

EBTNodeResult::Type UBTTask_ChangeBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();
	if(!BBC) return EBTNodeResult::Failed;

	const FName KeyName = KeySelector.SelectedKeyName;

	if(bIsBool)
	{
		BBC->SetValueAsBool(KeyName, BoolValue);
	}
	else if(bIsVector)
	{
		BBC->SetValueAsVector(KeyName, VectorValue);
	}
	else if(bIsNpcStateEnum)
	{
		ACoreNpc* Npc = Cast<ACoreNpc>(OwnerComp.GetAIOwner()->GetPawn());
		if(!Npc)
			return EBTNodeResult::Failed;

		Npc->ChangeNpcState(NpcStateEnumValue);
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}
