// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreSaveCharacter.h"
#include "ProjectT/Data/Gen/GenerateEnumObjectiveSubTypes.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/QuestManager.h"
#include "ProjectT/System/SinglePlay/GameModes/SPGameModeBase.h"

ACoreSaveCharacter::ACoreSaveCharacter():
	ObjectiveType(EObjectiveSubTypes::Save)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACoreSaveCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACoreSaveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoreSaveCharacter::EndAction_Implementation()
{
	Super::EndAction_Implementation();

	const UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	ASPGameModeBase* GM = UGlobalUtilsLibrary::GetGameModeChecked<ASPGameModeBase>(World);

	UQuestManager* QM = IWorldObjectProvider::Execute_GetQuestManager(GM);
	NMT_CHECKF(QM);

	QM->CheckObjectiveSatisfied(ObjectiveType, 0);
}
