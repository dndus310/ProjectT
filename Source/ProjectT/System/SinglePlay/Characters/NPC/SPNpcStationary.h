// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"
#include "SPNpcStationary.generated.h"

UCLASS()
class PROJECTT_API ASPNpcStationary : public ASPNpcAgent
{
	GENERATED_BODY()

public:
	ASPNpcStationary();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ActivateCharacter(const bool InbEnable) override;
	virtual void ChangeNpcState(const ENpcState InState) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Respawn() override;
	
};