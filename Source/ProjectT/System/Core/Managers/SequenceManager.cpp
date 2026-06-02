// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceManager.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/NotificationCenter.h"


USequenceManager::USequenceManager() :
	bSkipable(true),
	SequenceActor(nullptr),
	SequencePlayer(nullptr),
	CurrentSequence(nullptr),	
	SkipOffset(0.1f)	
{
	
}

void USequenceManager::InitializeSequenceManager(UWorld* World)
{
	
}

void USequenceManager::UpdateSequence(UWorld* World, const FName& RowName, const FOnEndSequence& Callback)
{
	if(CHECK_FREE(RowName.ToString()))
	{
		Callback.Execute();
		return;
	}
	
	FMovieSceneSequencePlaybackSettings NewSettings;
	
	if(!SequencePlayer)
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(World, CurrentSequence.Get(), NewSettings, SequenceActor);
	
	if(!NMT_ENSURE(CurrentSequence && SequencePlayer)) return;
	
	if(SequencePlayer->OnFinished.IsBound())
		SequencePlayer->OnFinished.RemoveAll(this);
	
	SequencePlayer->OnFinished.AddDynamic(this, &USequenceManager::OnFinishedSequence);
	
	if(OnEndSequence.IsBound())
		OnEndSequence.Unbind();
	
	OnEndSequence = Callback;
	
	SequencePlayer->Stop();
	SequenceActor->SetSequence(CurrentSequence);
	SequencePlayer->Play();
	OnSceneEvent.Broadcast(SequencePlayer->GetSequenceName());
	if(!bSkipable || !OnSequenceStarted.IsBound()) return;
	OnSequenceStarted.Execute();
	
	UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(UGameplayStatics::GetGameInstance(World));
	//NC->Post(ENotificationTypes::Sequence_Begin);
}

void USequenceManager::UpdateSequenceWithPlayRate(UWorld* World, const FName& RowName, float PlayRate,
	FOnEndSequence Callback)
{
	if(CHECK_FREE(RowName.ToString()))
	{
		Callback.Execute();
		return;
	}
}

void USequenceManager::ReleaseManager()
{
	NOT_IMPLEMENTED();
}

void USequenceManager::OnFinishedSequence()
{
	if(SequencePlayer) SequencePlayer->SetPlayRate(1.0f);

	UNotificationCenter* NC = IGlobalObjectProvider::Execute_GetNotificationCenter(UGameplayStatics::GetGameInstance(this));
	
	if(!NMT_ENSURE(OnEndSequence.IsBound())) return;
	OnEndSequence.Execute();
	if(!OnSequenceFinishedWithoutSkip.IsBound()) return;
	OnSequenceFinishedWithoutSkip.Execute();
}

void USequenceManager::StopSequence() const
{
	if(SequencePlayer && SequencePlayer->IsPlaying())
		SequencePlayer->Stop();
}

void USequenceManager::AddSceneEventListener(TFunction<void(const FString&)> Func)
{
	OnSceneEvent.AddLambda([Func](const FString& SequenceName)
	{
		Func(SequenceName);
	});
}

void USequenceManager::SkipCurrentSequence()
{	
	FQualifiedFrameTime FrameTime = SequencePlayer->GetEndTime();
	float PostSkipTime = FrameTime.AsSeconds() - SkipOffset;
	
	UMovieSceneSequence* Sequence = SequencePlayer->GetSequence();
	NMT_ENSURE(Sequence);
	TArray<FMovieSceneMarkedFrame> MarkedFrames = Sequence->GetMovieScene()->GetMarkedFrames();	
	if(MarkedFrames.Num() > 0)
	{
		int Idx = MarkedFrames.IndexOfByPredicate([&](FMovieSceneMarkedFrame MarkedFrame){
			return MarkedFrame.Label.Contains(TEXT("PostSkipTime"));
		});
		if (Idx >= 0)
		{
			float TimeScaleFactor = 800.f; 
			PostSkipTime = MarkedFrames[Idx].FrameNumber / SequencePlayer->GetFrameRate();			
			PostSkipTime /= TimeScaleFactor;
		}
	}
	
	FMovieSceneSequencePlaybackParams PlaybackParams;
	PlaybackParams.Time = PostSkipTime;
	PlaybackParams.PositionType = EMovieScenePositionType::Time;
	
	SequencePlayer->SetPlaybackPosition(PlaybackParams);
	SequencePlayer->Play();	
}

bool USequenceManager::IsCurrentSequenceSkipable()
{
	FQualifiedFrameTime FrameTime = SequencePlayer->GetEndTime();
	float Duration = FrameTime.AsSeconds();
	float CurrentTime = SequencePlayer->GetCurrentTime().AsSeconds();
	
	return Duration - CurrentTime >= 1.0f;
}

void USequenceManager::BeginDestroy()
{
	UObject::BeginDestroy();

	OnEndSequence.Unbind();
	OnSceneEvent.Clear();
}



