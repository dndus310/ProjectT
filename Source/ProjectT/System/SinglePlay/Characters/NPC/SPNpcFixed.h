// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"
#include "SPNpcFixed.generated.h"

UCLASS()
class PROJECTT_API ASPNpcFixed : public ACoreNpc, public IAISettings
{
	GENERATED_BODY()

public:
	ASPNpcFixed();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual UBlackboardData* GetBlackboardData_Implementation() override;
	virtual UBehaviorTree* GetBehaviorTree_Implementation() override;
	virtual void StartDisableNpc() override;
	virtual void CompleteDisableNpc() override;
	virtual void ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects) override;;
	virtual void ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects) override;
	virtual void ActivateCharacter(const bool InbEnable) override;
	virtual void SetDetectTargetFromNoise(AActor* InTarget) override {};
	virtual void SetDetectLocationFromNoise(const FVector& InTargetLocation) override {};
	virtual void SetSpringArmRotation() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitializeProperty() override;
	
#if WITH_EDITOR
	virtual void ApplyCharacterData(const FTableRowBase* RowData) override;
#endif

	UFUNCTION()
	void DetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void DetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetDetectSpotLocation();
	void SetDetectMaterial(const bool InbDetect) const;
	void UpdateDetectState(const bool InbDetect, const class ACorePlayerCharacter* InTarget);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadPitchAngle;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class USphereComponent> DetectCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UStaticMeshComponent> DetectSpotMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UStaticMeshComponent> DetectConeMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UNpcFixedAnimInstance> FixedAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UBoxComponent> BoundingBoxComponent;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DetectSpotDynamicMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DetectConeDynamicMaterial;

	FName DetectParameterName;
};
