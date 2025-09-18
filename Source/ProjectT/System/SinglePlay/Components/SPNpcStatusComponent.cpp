// Fill out your copyright notice in the Description page of Project Settings.

#include "SPNpcStatusComponent.h"

#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"

USPNpcStatusComponent::USPNpcStatusComponent() :
	bOverrideStatusInEditor(false),
	HalfRotationAngle(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

/**
 * NOTE: ApplyDamage Description
 * @param InDamageAmount: Damage Hit Value 
 * @return 'True': Npc Is Dead
 */
bool USPNpcStatusComponent::ApplyDamage(const float InDamageAmount)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	if(CCM->GetDebugStatus().bDebugEnemyInvincibleHP) return false;
	
	if(CurrentNpcStatus.fNpcHp <= 0.f)
		return true;
	
	CurrentNpcStatus.fNpcHp = CurrentNpcStatus.fNpcHp - InDamageAmount;
	if(OnNpcHit.IsBound())
		CurrentNpcStatus.fNpcHp < 0.f ? OnNpcHit.Broadcast(0.f) : OnNpcHit.Broadcast(CurrentNpcStatus.fNpcHp / PreviousNpcStatus.fNpcHp);
	
	NMT_LOGF("Damage : %f", InDamageAmount)
	return CurrentNpcStatus.fNpcHp <= 0.f;
}

void USPNpcStatusComponent::UpdateCurrentDoubtTime(const float InDoubtTime)
{
	CurrentNpcStatus.fDoubtTime = InDoubtTime;
}

FEnemyNpc USPNpcStatusComponent::GetPreviousNpcStatus()
{
	return PreviousNpcStatus;
}

FEnemyNpc USPNpcStatusComponent::GetCurrentNpcStatus()
{
	return CurrentNpcStatus;
}

float USPNpcStatusComponent::GetStatusPreviousNpcHp()
{
	return PreviousNpcStatus.fNpcHp;
}

float USPNpcStatusComponent::GetStatusNpcHp()
{
	return CurrentNpcStatus.fNpcHp;
}

float USPNpcStatusComponent::GetStatusPreviousNpcMove()
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	return PreviousNpcStatus.fNpcMove * (CCM->GetDebugStatus().bDebugEnemySpeed ? 2.f : 1.f);
}

float USPNpcStatusComponent::GetStatusNpcMove()
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	return CurrentNpcStatus.fNpcMove * (CCM->GetDebugStatus().bDebugEnemySpeed ? 2.f : 1.f);
}

void USPNpcStatusComponent::SetStatusNpcMove(float InValue)
{
	CurrentNpcStatus.fNpcMove = InValue;
}

void USPNpcStatusComponent::AddStatusNpcMove(const float InValue)
{
	CurrentNpcStatus.fNpcMove = GetStatusNpcMove() + InValue;
}

float USPNpcStatusComponent::GetStatusPreviousNpcRun()
{
	return PreviousNpcStatus.fNpcRun;
}

float USPNpcStatusComponent::GetStatusNpcRun()
{
	return CurrentNpcStatus.fNpcRun;
}

void USPNpcStatusComponent::SetStatusNpcRun(float InValue)
{
	CurrentNpcStatus.fNpcRun = InValue;
}

void USPNpcStatusComponent::AddStatusNpcRun(const float InValue)
{
	CurrentNpcStatus.fNpcRun = GetStatusNpcRun() + InValue;
}

float USPNpcStatusComponent::GetStatusNpcSightLength()
{
	return CurrentNpcStatus.fSightLength;
}

float USPNpcStatusComponent::GetStatusNpcSightAngle()
{
	return CurrentNpcStatus.fSightAngle;
}

float USPNpcStatusComponent::GetStatusNpcAttackLength()
{
	return CurrentNpcStatus.fAttackLength;
}

float USPNpcStatusComponent::GetStatusNpcChaseLength()
{
	return CurrentNpcStatus.fChaseLength;
}

float USPNpcStatusComponent::GetStatusNpcRotationAngle()
{
	return CurrentNpcStatus.fRotationAngle;
}

float USPNpcStatusComponent::GetStatusNpcHalfRotationAngle()
{
	return HalfRotationAngle;
}

float USPNpcStatusComponent::GetStatusNpcRotationSpeed()
{
	return CurrentNpcStatus.fRotationSpeed;
}

void USPNpcStatusComponent::SetStatusNpcRotationSpeed(float InValue)
{
	CurrentNpcStatus.fRotationSpeed = InValue;
}

float USPNpcStatusComponent::GetStatusNpcDoubtTime()
{
	return CurrentNpcStatus.fDoubtTime;
}

float USPNpcStatusComponent::GetStatusPreviousNpcDoubtTime()
{
	return PreviousNpcStatus.fDoubtTime;
}

float USPNpcStatusComponent::GetStatusNpcNoise()
{
	return CurrentNpcStatus.fNpcNoise;
}

float USPNpcStatusComponent::GetStatusNpcMoveProb()
{
	return CurrentNpcStatus.fMoveProb;
}

float USPNpcStatusComponent::GetStatusNpcSearchProb()
{
	return CurrentNpcStatus.fSearchProb;
}

float USPNpcStatusComponent::GetStatusNpcScoutProb()
{
	return CurrentNpcStatus.fScoutProb;
}

int32 USPNpcStatusComponent::GetStatusNpcAttackId()
{
	return CurrentNpcStatus.iNpcAttackId;
}

void USPNpcStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USPNpcStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USPNpcStatusComponent::InitializeStatus(const FEnemyNpc& InStatus)
{
	PreviousNpcStatus = InStatus;
	CurrentNpcStatus = InStatus;
	HalfRotationAngle = InStatus.fRotationAngle / 2.f;
	CurrentNpcStatus.fDoubtTime = 0.f;
}

void USPNpcStatusComponent::ReInitializeStatus()
{
	CurrentNpcStatus = PreviousNpcStatus;
	CurrentNpcStatus.fDoubtTime = 0.f;
	HalfRotationAngle = CurrentNpcStatus.fRotationAngle / 2.f;
}
