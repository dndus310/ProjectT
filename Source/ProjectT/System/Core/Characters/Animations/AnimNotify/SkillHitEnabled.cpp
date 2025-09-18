// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillHitEnabled.h"

#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Characters/Skill/CoreSkill.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"

void USkillHitEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if(const ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(MeshComp->GetOwner()))
	{
		ACoreSkill* LocCurrentSkill;
		LocPlayerCharacter->GetCurrentSkill(LocCurrentSkill);
		if (LocCurrentSkill)
			LocCurrentSkill->SkillHitEnable(true);
		return;
	}

	if(const ASPNpcAgent* LocASPNpcAgent = Cast<ASPNpcAgent>(MeshComp->GetOwner()))
	{
		ACoreSkill* LocCurrentSkill = LocASPNpcAgent->GetCurrentSkill();
		if (LocCurrentSkill)
			LocCurrentSkill->SkillHitEnable(true);
		
		return;
	}

	if(ACoreSkill* LocACoreSkill = Cast<ACoreSkill>(MeshComp->GetOwner()))
	{
		LocACoreSkill->SkillObjectHitEnable(true);
	}
}
