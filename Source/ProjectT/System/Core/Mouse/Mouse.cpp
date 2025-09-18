// Fill out your copyright notice in the Description page of Project Settings.


#include "Mouse.h"

#include "LinearVisualObject.h"
#include "ParabolaVisualObject.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/MouseInteraction.h"

UMouse::UMouse() :
	bTickEnabled(false),
	bMouseTracing(false),
	Range(0),
	CurrentResultType(),
	LinearTraceQuery(),
	MouseTraceQuery(),
	StandardActor(nullptr),
	CurrentPlayerController(nullptr)
{
	OnConstruct();
}

void UMouse::Tick(float DeltaTime)
{
	if(bMouseTracing)
	{
		FHitResult LocHitResult;
		switch(CurrentResultType)
		{
			case EMouseResultType::Linear :
			{
				LocHitResult = GetLinearCalibrateHitResult();
				IMouseInteraction::Execute_RefreshCursorHitResult(LinearVisualObject, LocHitResult);
				break;
			}
			case EMouseResultType::Parabola :
			{
				LocHitResult = GetParabolaCalibrateHitResult();
				IMouseInteraction::Execute_RefreshCursorHitResult(ParabolaVisualObject, LocHitResult);
				break;
			}
			default: NMT_LOG("ResultType None") break;
		}
		for(AActor* LocAffectActor : AffectActors)
		{
			IMouseInteraction::Execute_RefreshCursorHitResult(LocAffectActor, LocHitResult);			
		}
	}
}

TStatId UMouse::GetStatId() const
{
	return TStatId();
}

bool UMouse::IsTickable() const
{
	return bTickEnabled;
}

void UMouse::SetTickEnabled(const bool InbEnabled)
{
	bTickEnabled = InbEnabled;
}

void UMouse::InitializeMouse()
{
	if(UWorld* LocWorld = UGlobalUtilsLibrary::GetValidWorld(this))
	{
		if(APlayerController* LocGetController = UGameplayStatics::GetPlayerController(LocWorld,0))
		{
			CurrentPlayerController = LocGetController;
			LinearTraceQuery = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3);
			MouseTraceQuery = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1);

			LinearVisualObject = LocWorld->SpawnActor<ALinearVisualObject>(LinearVisualObjectClass);
			LinearVisualObject->InitializeLinearObject();

			ParabolaVisualObject = LocWorld->SpawnActor<AParabolaVisualObject>(ParabolaVisualObjectClass);
			ParabolaVisualObject->InitializeParabolaObject();
		}
	}	
}

void UMouse::OnConstruct()
{
	//TODO : Edit Path Relative
	FString LocTempPath = "/Script/Engine.Blueprint'/Game/System/Core/Mouse/Blueprints/BP_LinearVisualObject.BP_LinearVisualObject'";
	FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(LocTempPath);
	UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
	NMT_CHECKF(LoadedClass);
	LinearVisualObjectClass = LoadedClass;
	LoadedClass = nullptr;
	
	LocTempPath = "/Script/Engine.Blueprint'/Game/System/Core/Mouse/Blueprints/BP_ParabolaVisualObject.BP_ParabolaVisualObject'";
	ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(LocTempPath);
	LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
	NMT_CHECKF(LoadedClass);
	ParabolaVisualObjectClass = LoadedClass;
}

void UMouse::OnEndPlay()
{
	SetTickEnabled(false);
}

void UMouse::AimTracing(bool InbStart, AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const float InAffectRange, const EMouseResultType InResultType, bool InbPathVisible)
{
	if(InbStart)
	{
		StandardActor = InStandardActor;
		AffectActors = InAffectActors;
		Range = InRange;
		CurrentResultType = InResultType;
		bMouseTracing = true;
		SetTickEnabled(true);
		switch(InResultType)
		{
			case EMouseResultType::Parabola:
			{
				ParabolaVisualObject->SetAimVisibility(true, InbPathVisible, InAffectRange);
				break;
			}
			case EMouseResultType::Linear:
			{
				LinearVisualObject->SetAimVisibility(true);
				break;
			}
			default: NMT_LOG("MouseHitResult Type None");
		}
	}
	else
	{
		ParabolaVisualObject->SetAimVisibility(false);
		LinearVisualObject->SetAimVisibility(false);
		SetTickEnabled(false);	
		bMouseTracing = false;
		StandardActor = nullptr;
		AffectActors.Empty();
		Range = 0.f;
		CurrentResultType = EMouseResultType::None;
	}	
}

FHitResult UMouse::GetCursorHitResult(AActor* InStandardActor, const TArray<AActor*>& InAffectActors, const float InRange, const EMouseResultType InResultType)
{
	StandardActor = InStandardActor;
	AffectActors = InAffectActors;
	Range = InRange;
	FHitResult LocHitResult;
	switch(InResultType)
	{
		case EMouseResultType::Linear :
		{
			LocHitResult = GetLinearCalibrateHitResult();
			break;
		}
		case EMouseResultType::Parabola :
		{
			LocHitResult = GetParabolaCalibrateHitResult();
			break;
		}
		default: NMT_LOG("ResultType None") break;
	}
	return LocHitResult;
}

FHitResult UMouse::GetParabolaCalibrateHitResult()
{
	FHitResult LocCursorHitResult;
	CurrentPlayerController->GetHitResultUnderCursorByChannel(MouseTraceQuery, true, MouseCursorHitResult);
	if(MouseCursorHitResult.bBlockingHit)
	{
		float LocCursorDistance = FVector::Dist2D(StandardActor->GetActorLocation(), MouseCursorHitResult.Location);
		float LocTraceLength = LocCursorDistance > Range ? LocTraceLength = Range : LocTraceLength = LocCursorDistance;
		FVector LocHitForward(MouseCursorHitResult.Location.X, MouseCursorHitResult.Location.Y, StandardActor->GetActorLocation().Z);
		LocHitForward = LocHitForward - StandardActor->GetActorLocation();
		LocHitForward.Normalize();
		TArray<AActor*> LocIgnoreActors;
		LocIgnoreActors.Add(StandardActor);

		FHitResult LocTopDownHitResult;
		UKismetSystemLibrary::LineTraceSingle(this, FVector(0.f, 0.f, 300.f) + StandardActor->GetActorLocation() + LocHitForward * LocTraceLength,
											  FVector(0.f, 0.f, -100.f) + StandardActor->GetActorLocation() + LocHitForward * LocTraceLength, LinearTraceQuery, false, LocIgnoreActors,
											  EDrawDebugTrace::None, LocTopDownHitResult, true);
		if(LocTopDownHitResult.bBlockingHit)
		{
			FHitResult LocWallHitResult;
			TArray<TEnumAsByte<EObjectTypeQuery>> LocWallObjectType;
			LocWallObjectType.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel4));
			UKismetSystemLibrary::LineTraceSingleForObjects(this, StandardActor->GetActorLocation(), StandardActor->GetActorLocation() + LocHitForward * LocTraceLength, LocWallObjectType, false, LocIgnoreActors,
															EDrawDebugTrace::None, LocWallHitResult, true);
			if(LocWallHitResult.bBlockingHit)
			{
				LocTopDownHitResult.Component = LocWallHitResult.Component;
				LocTopDownHitResult.Location = LocWallHitResult.Location;
				LocTopDownHitResult.Location.Z = MouseCursorHitResult.Location.Z;
				LocTraceLength = LocWallHitResult.Distance;
			}
			
		}
		TArray<FHitResult> LocPropHitResults;
		TArray<TEnumAsByte<EObjectTypeQuery>> LocPropObjectType;
		LocPropObjectType.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel6));
		UKismetSystemLibrary::LineTraceMultiForObjects(this, StandardActor->GetActorLocation(), StandardActor->GetActorLocation() + LocHitForward * LocTraceLength, LocPropObjectType, false, LocIgnoreActors,
													EDrawDebugTrace::None, LocPropHitResults, true);
		if(LocPropHitResults.Num() > 0)
		{
			if(LocTopDownHitResult.Component->GetCollisionProfileName() == TEXT("Ground")||LocTopDownHitResult.Component->GetCollisionProfileName() == TEXT("Wall"))
			{
				LocCursorHitResult = LocTopDownHitResult;
			}
			else
			{
				LocPropHitResults.Last().Location.Z = MouseCursorHitResult.Location.Z;
				LocCursorHitResult = LocPropHitResults.Last();				
			}
		}
		else
		{
			LocTopDownHitResult.Location.Z = MouseCursorHitResult.Location.Z;
			LocCursorHitResult = LocTopDownHitResult;
		}
		LastUpdatedHitResult = LocCursorHitResult;
	}
	else
	{
		LocCursorHitResult = LastUpdatedHitResult;
	}
	return LocCursorHitResult;
}

FHitResult UMouse::GetLinearCalibrateHitResult()
{
	FHitResult LocCursorHitResult;
	CurrentPlayerController->GetHitResultUnderCursorByChannel(MouseTraceQuery, true, MouseCursorHitResult);
	if(MouseCursorHitResult.bBlockingHit)
	{
		FVector LocHitForward(MouseCursorHitResult.Location.X, MouseCursorHitResult.Location.Y, StandardActor->GetActorLocation().Z);
		LocHitForward = LocHitForward - StandardActor->GetActorLocation();
		LocHitForward.Normalize();
		TArray<AActor*> LocIgnoreActors;
        LocIgnoreActors.Add(StandardActor);
		UKismetSystemLibrary::LineTraceSingle(this, StandardActor->GetActorLocation(), StandardActor->GetActorLocation() + LocHitForward * Range,
											   LinearTraceQuery, false, LocIgnoreActors, EDrawDebugTrace::None, LocCursorHitResult, true);
		if(!LocCursorHitResult.bBlockingHit)
		{
			LocCursorHitResult.Location = MouseCursorHitResult.Location;
			LocCursorHitResult.Distance = Range;
		}
		LastUpdatedHitResult = LocCursorHitResult;
	}
	else
	{
		LocCursorHitResult = LastUpdatedHitResult;
	}
	return LocCursorHitResult; 
}