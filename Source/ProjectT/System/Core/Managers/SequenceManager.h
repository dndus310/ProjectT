// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SequenceManager.generated.h"


/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSceneEvent, const FString&)
DECLARE_DELEGATE(FOnSequenceStarted);
DECLARE_DELEGATE(FOnSequenceFinishedWithoutSkip);
DECLARE_DELEGATE(FOnEndSequence);

class ALevelSequenceActor;
class ULevelSequencePlayer;
class ULevelSequence;

UCLASS(BlueprintType)
class PROJECTT_API USequenceManager : public UObject
{
	GENERATED_BODY()

public:
	USequenceManager();

	UFUNCTION()
	void OnFinishedSequence();
	
	void InitializeSequenceManager(UWorld* World);
	void UpdateSequence(UWorld* World, const FName& RowName, const FOnEndSequence& Callback);
	void UpdateSequenceWithPlayRate(UWorld* World, const FName& RowName, float PlayRate, FOnEndSequence Callback);
	void ReleaseManager();
	
	void StopSequence() const;
	void AddSceneEventListener(TFunction<void(const FString&)> Func);

	UFUNCTION(BlueprintCallable)
	void SkipCurrentSequence();

	bool IsCurrentSequenceSkipable();
	
	FOnSequenceFinishedWithoutSkip OnSequenceFinishedWithoutSkip;
	FOnSequenceStarted OnSequenceStarted;
	uint8 bSkipable:1;
	
protected:
	virtual void BeginDestroy() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	ALevelSequenceActor* SequenceActor;

	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;

	UPROPERTY()
	TObjectPtr<ULevelSequence> CurrentSequence;
	
	FOnEndSequence OnEndSequence;
	FOnSceneEvent OnSceneEvent;
	FName CurrentSequenceName;	
	float SkipOffset;

};

