// Fill out your copyright notice in the Description page of Project Settings.


#include "PooledActorManager.h"

#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/PoolableActor.h"

UPooledActorManager::UPooledActorManager()
{
}

void UPooledActorManager::CreatePool(const TSubclassOf<AActor>& ActorClass, const int32 PoolSize, TArray<AActor*>* OutActors)
{
	NMT_CHECKF(ActorClass);

	bool bImplemented = ActorClass->ImplementsInterface(UPoolableActor::StaticClass());
	NMT_MSG_CHECKF(bImplemented, "ActorClass does not implement the required IPoolableActor interface.");

	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	TArray<AActor*>& Values = Pool.FindOrAdd(ActorClass);

	for(int32 i = 0; i < PoolSize; ++i)
	{
		AActor* NewActor = World->SpawnActor<AActor>(ActorClass);
		IPoolableActor::Execute_Disable(NewActor);
		Values.Push(NewActor);
	}
	
	if(OutActors != nullptr)
	*OutActors = Values;
}

void UPooledActorManager::Push(AActor* Object)
{
	NMT_CHECKF(Object);

	TSubclassOf<AActor> ActorClass = Object->GetClass();
	bool bClassContains = Pool.Contains(ActorClass);
	NMT_MSG_CHECKF(bClassContains, "Not found class type in pool");

	TArray<AActor*>* FoundValues = Pool.Find(ActorClass);
	NMT_CHECKF(FoundValues);

	IPoolableActor::Execute_Pushed(Object);
	IPoolableActor::Execute_Disable(Object);

	if (!FoundValues->Contains(Object))	FoundValues->Push(Object);
}

AActor* UPooledActorManager::Create(const TSubclassOf<AActor>& ActorClass)
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	TArray<AActor*>* FoundValues = Pool.Find(ActorClass);
	NMT_CHECKF(FoundValues);

	AActor* NewActor = World->SpawnActor<AActor>(ActorClass);
	return NewActor;
}

AActor* UPooledActorManager::Obtain(const TSubclassOf<AActor>& ActorClass)
{
	NMT_CHECKF(ActorClass);

	bool bImplemented = ActorClass->ImplementsInterface(UPoolableActor::StaticClass());
	NMT_MSG_CHECKF(bImplemented, "ActorClass does not implement the required IPoolableActor interface.");

	bool bClassContains = Pool.Contains(ActorClass);
	NMT_MSG_CHECKF(bClassContains, "Not found class type in pool");

	TArray<AActor*>* FoundValues = Pool.Find(ActorClass);
	NMT_CHECKF(FoundValues);

	AActor* Ret = nullptr;
	if(FoundValues->IsEmpty()) Ret = Create(ActorClass);
	else
	{
		Ret = FoundValues->Pop();
		if(!NMT_MSG_ENSURE(Ret, "Actor is null please check"))
			Ret = Create(ActorClass);
	}
	
	IPoolableActor::Execute_Obtained(Ret);
	IPoolableActor::Execute_Enable(Ret);
	return Ret;
}

void UPooledActorManager::ReleaseManager()
{
	for(TTuple<TSubclassOf<AActor>, TArray<AActor*>>& It : Pool)
	{
		for(AActor*& Value : It.Value)
			if(Value) Value = nullptr;

		It.Value.Empty();
	}
	Pool.Empty();
}
