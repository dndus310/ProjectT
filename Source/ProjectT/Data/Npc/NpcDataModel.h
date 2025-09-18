// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NpcDataModel.generated.h"

USTRUCT(Blueprintable)
struct FCrowdSettingsInfo
{
	GENERATED_BODY()

	FCrowdSettingsInfo() :
		bUseCrowdSeparation(true),
		SeparationWeight(10.f),
		AvoidanceRangeMultiplier(1.f)
	{}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrowdSettings")
	uint8 bUseCrowdSeparation : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrowdSettings", meta = (EditCondition = "bUseCrowdSeparation"), meta = (Tooltip = "NPC 간 거리 유지 강도"))
	float SeparationWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrowdSettings", meta = (EditCondition = "bUseCrowdSeparation"), meta = (Tooltip = "회피 반응 배율"))
	float AvoidanceRangeMultiplier;
};

UENUM(BlueprintType)
enum class EHeadRotationType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Search UMETA(DisplayName = "Search"),
	Guard UMETA(DisplayName = "Guard"),
	LoopSearch UMETA(DisplayName = "LoopSearch"),
	Pause UMETA(DisplayName = "Pause"),
	TurnToTarget UMETA(DisplayName = "TurnToTarget")
};

UENUM(BlueprintType)
enum class ENpcState : uint8
{
	Default = 0 UMETA(DisplayName = "Default"),
	Returning UMETA(DisplayName = "Returning"),
	Doubt UMETA(DisplayName = "Doubt"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack"),
	Guard UMETA(DisplayName = "Guard"),
	Hit UMETA(DisplayName = "Hit"),
	Dead UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum class ENpcSpeedType : uint8
{
	Walk = 0 UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Stop UMETA(DisplayName = "Stop"),
};

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Patrol UMETA(DisplayName = "Patrol"),
	Search UMETA(DisplayName = "Search"),
	SearchPatrol UMETA(DisplayName = "SearchPatrol"),
};

UENUM(BlueprintType)
enum class EDoubtState : uint8
{	
	None = 0 UMETA(DisplayName = "None"),
	Increasing UMETA(DisplayName = "Increasing"),
	Decreasing UMETA(DisplayName = "Decreasing"),
};

UENUM(BlueprintType)
enum class EVisionMode : uint8
{	
	None = 0 UMETA(DisplayName = "None"),
	Cone UMETA(DisplayName = "Cone"),
	Line UMETA(DisplayName = "Line"),
};

UENUM(BlueprintType)
enum class ENpcNormalAnimState : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Idle UMETA(DisplayName = "Idle"),
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
};

UENUM(BlueprintType)
enum class ENpcAbnormalAnimState : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	HitBack UMETA(DisplayName = "HitBack"),
	JumpBack UMETA(DisplayName = "JumpBack"),
	FallDown UMETA(DisplayName = "FallDown"),
	GetUp UMETA(DisplayName = "GetUp"),
	Stun UMETA(DisplayName = "Stun"),
	DeadStart UMETA(DisplayName = "DeadStart"),
	DeadLoop UMETA(DisplayName = "DeadLoop"),
};

USTRUCT(Blueprintable)
struct FNpcSimpleRatio
{
	GENERATED_BODY()
	FNpcSimpleRatio() :
		CurrentValue(0.f),
		MaxValue(0.f)
	{		
	}
	
	UPROPERTY(EditAnywhere)
	float CurrentValue;

	UPROPERTY(EditAnywhere)
	float MaxValue;
};

USTRUCT(Blueprintable)
struct FNpcEffectValue
{
	GENERATED_BODY()
	FNpcEffectValue() :
		RemainTime(0.f),
		MaxTime(0.f),
		EffectValue(0.f)
	{		
	}
	
	UPROPERTY(EditAnywhere)
	float RemainTime;

	UPROPERTY(EditAnywhere)
	float MaxTime;

	UPROPERTY(EditAnywhere)
	float EffectValue;
};

UCLASS()
class PROJECTT_API UNpcDataModel : public UObject
{
	GENERATED_BODY()
	
};
