// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreInteractionObject.h"
#include "ProjectT/Data/Gen/GenerateStructLevelObject.h"
#include "ProjectT/Data/Gen/GenerateEnumLevelObjectTypes.h"
#include "LevelInteractionObject.generated.h"

UCLASS()
class PROJECTT_API ALevelInteractionObject : public ACoreInteractionObject
{
	GENERATED_BODY()

public:
	ALevelInteractionObject();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void SetVisibility(bool InbVisible) override;
	virtual void ShowVisual_Implementation() override;
	virtual void HideVisual_Implementation() override;
	virtual void Destroyed() override;

	//FIXME
	int32 GetCastItemID() { return LevelObjectData.iCastItemId; }
protected:
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void ApplyObjectConfiguration(const FName& RowName);	
#endif

	void ExecuteBindNPCEvent();
	void ExecuteBindCeilEvent();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> BoxCollision;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	FLevelObject LevelObjectData;	

private:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class ACoreNpc>> BindNPCs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class ACeilObject>> CeilObjects;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TArray<TObjectPtr<class ACoreNpc>> PrevSelectedNPCs;
#endif
};
