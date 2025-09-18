// Fill out your copyright notice in the Description page of Project Settings.


#include "NonInputAnimEnd.h"

#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Characters/Player/Animations/AnimInstance/PlayerCharacterAnimInstance.h"

void UNonInputAnimEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ACorePlayerCharacter* LocPlayerCharacter = Cast<ACorePlayerCharacter>(MeshComp->GetOwner());
	if(LocPlayerCharacter)
	{
		UPlayerCharacterAnimInstance* LocPlayerAnimInstance;
		LocPlayerCharacter->GetAnimInstance(LocPlayerAnimInstance);
		LocPlayerAnimInstance->NonInputAnimFinished();
		return;
	}
}
