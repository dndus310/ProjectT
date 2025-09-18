// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "UObject/Interface.h"
#include "AISettings.generated.h"

USTRUCT(BlueprintType)
struct FSightInfo
{
	GENERATED_BODY()

	FSightInfo() :
		SightRange(300.f),
		LoseSightRange(450.f),
		SightAngle(90.f),
		SightMaxAge(3.f)
	{}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float SightRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float LoseSightRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float SightAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float SightMaxAge;
};

USTRUCT(BlueprintType)
struct FHearingInfo
{
	GENERATED_BODY()

	FHearingInfo() :
		HearingRange(300.f),
		HearingMaxAge(3.f)
	{}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Hearing")
	float HearingRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Hearing")
	float HearingMaxAge;
};


UINTERFACE(MinimalAPI)
class UAISettings : public UInterface
{
	GENERATED_BODY()
};

class PROJECTT_API IAISettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting|Perception")
	bool GetUseSight();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting|Perception")
	bool GetUseHearing();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting|Perception")
	FSightInfo GetSightInfo();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting|Perception")	
	FHearingInfo GetHearingInfo();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting")
	class UBehaviorTree* GetBehaviorTree();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting")
	class UBlackboardData* GetBlackboardData();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting|Perception")
	void DetectTargetBySight(AActor* InTarget, FAIStimulus InStimulus);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting|Perception")
	void ForgetTarget(AActor* InTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AISetting")
	FTransform GetViewTransform();
};
