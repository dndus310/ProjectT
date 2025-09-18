// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreSkill.h"
#include "PyroAtk.generated.h"

UCLASS()
class PROJECTT_API APyroAtk : public ACoreSkill
{
	GENERATED_BODY()

public:
	APyroAtk();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
	virtual void Pushed_Implementation() override;
	virtual void Obtained_Implementation() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void StartSkill() override;
	virtual void CancelSkill() override;
	virtual void FinishSkill() override;
	virtual void SkillHitEnable(bool InbEnable) override;
	virtual void SkillFXEnable(bool InbEnable, int32 InIndex = 0) override;
	
	UFUNCTION()
	void BoxCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PyroAtkRootSceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> PyroAtkCollisionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNiagaraComponent> PyroAtkFXComponent;
};
