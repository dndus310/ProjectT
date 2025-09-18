// Fill out your copyright notice in the Description page of Project Settings.


#include "AbnormalAnimEnd.h"

#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

void UAbnormalAnimEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ASPNpcAgent* AgentNpc = Cast<ASPNpcAgent>(MeshComp->GetOwner());
	if (AgentNpc)
	{
		AgentNpc->DetermineNextStateAfterAbnormal();
	}
}
