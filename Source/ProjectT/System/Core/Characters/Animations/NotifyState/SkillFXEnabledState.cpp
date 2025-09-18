// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFXEnabledState.h"

#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Characters/Skill/CoreSkill.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

void USkillFXEnabledState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(MeshComp->GetOwner());
	if(LocPlayerCharacter)
	{
		ACoreSkill* LocCurrentSkill;
		LocPlayerCharacter->GetCurrentSkill(LocCurrentSkill);
		if (LocCurrentSkill)
			LocCurrentSkill->SkillFXEnable(true, FXIndex);
		return;
	}

	ASPNpcAgent* LocASPNpcAgent = Cast<ASPNpcAgent>(MeshComp->GetOwner());
	if(LocASPNpcAgent)
	{
		ACoreSkill* LocCurrentSkill = LocASPNpcAgent->GetCurrentSkill();
		if (LocCurrentSkill)
			LocCurrentSkill->SkillFXEnable(true, FXIndex);
		return;
	}
}

void USkillFXEnabledState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(MeshComp->GetOwner());
	if(LocPlayerCharacter)
	{
		ACoreSkill* LocCurrentSkill;
		LocPlayerCharacter->GetCurrentSkill(LocCurrentSkill);
		if (LocCurrentSkill)
			LocCurrentSkill->SkillFXEnable(false, FXIndex);
		return;
	}

	ASPNpcAgent* LocASPNpcAgent = Cast<ASPNpcAgent>(MeshComp->GetOwner());
	if(LocASPNpcAgent)
	{
		ACoreSkill* LocCurrentSkill = LocASPNpcAgent->GetCurrentSkill();
		if (LocCurrentSkill)
			LocCurrentSkill->SkillFXEnable(false, FXIndex);
		return;
	}
}
