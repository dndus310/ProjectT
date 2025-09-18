// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreCharacter.h"
#include "ProjectT/System/Core/Characters/CoreCharacter.h"
#include "ProjectT/Data/Gen/GenerateStructCharacterMeshSet.h"
#include "ProjectT/System/Core/Interfaces/ContentType.h"
#include "ProjectT/System/Core/Interfaces/ObjectInteractionControl.h"
#include "CombatCharacter.generated.h"

class USkeletalMesh;
class UStaticMeshComponent;

UCLASS()
class PROJECTT_API ACombatCharacter : public ACoreCharacter, public IContentType
{
	GENERATED_BODY()

public:
	ACombatCharacter();
	virtual void PostLoad() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual EContentType GetContentType_Implementation() override;
	virtual FString GetContentTypeNumber_Implementation() override;
	void GetWeaponStaticComponent(TObjectPtr<UStaticMeshComponent>& OutStaticWeapon) const { OutStaticWeapon = WeaponStaticComponent; }

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void ApplyCharacterConfiguration(const FName& RowName);
	virtual bool ApplyCharacterSet(const FName& RowName);
	virtual void ApplyCharacterData(const FTableRowBase* RowData);
	void RestoreCharacterSet();
	void RestoreCharacterData();
#endif
	
	virtual void Respawn();

	virtual void ExecuteClickedEvent(UPrimitiveComponent* InTouchedComponent, FKey InButtonPressed) override;
	virtual void ExecuteBeginCursorOverEvent(UPrimitiveComponent* InTouchedComponent) override;
	virtual void ExecuteEndCursorOverEvent(UPrimitiveComponent* InTouchedComponent) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Design|CharacterSet")
	TObjectPtr<UStaticMeshComponent> AccessoryMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Design|CharacterSet")
	TObjectPtr<UStaticMeshComponent> WeaponStaticComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Design|CharacterSet")
	TObjectPtr<USkeletalMeshComponent> WeaponDynamicComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design|CharacterSet")
	FName CharacterRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design|CharacterSet", meta = (AllowPrivateAccess = "true"))
	FCharacterMeshSet CurrentMeshSet;
	
	UPROPERTY()
	TObjectPtr<USkeletalMesh> DefaultBodyMesh;

private:
	UPROPERTY()
	FName PrevCharacterRowName;
	
};
