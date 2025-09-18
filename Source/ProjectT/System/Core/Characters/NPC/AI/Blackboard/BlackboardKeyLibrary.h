// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlackboardKeyLibrary.generated.h"

namespace NpcBaseKeys
{
	extern const FName TargetActor;
	extern const FName TargetLocation;
	extern const FName TargetDir;
	extern const FName ReturnLocation;
	extern const FName CurrentState;
	extern const FName bCanAttack;
	extern const FName bCanRotateWhileAttack;
	extern const FName CurrentDoubtState;
	
	namespace PatrolKeys
	{
		extern FName bMoveReverse;
		extern FName PatrolType;
	}
}

UCLASS()
class PROJECTT_API UBlackboardKeyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (ToolTip = "TargetActor"))
	static FName GetTargetActorKey() {  return NpcBaseKeys::TargetActor; }

	UFUNCTION(BlueprintPure, meta = (ToolTip = "TargetLocation"))
	static FName GetTargetLocationKey() {  return NpcBaseKeys::TargetLocation; }

	UFUNCTION(BlueprintPure, meta = (ToolTip = "TargetDir"))
	static FName GetTargetDirKey() {  return NpcBaseKeys::TargetDir; }

	UFUNCTION(BlueprintPure, meta = (ToolTip = "ReturnLocation"))
	static FName GetReturnLocationKey() {  return NpcBaseKeys::ReturnLocation; }
	
	UFUNCTION(BlueprintPure, meta = (ToolTip = "NpcState"))
	static FName GetCurrentStateKey() {  return NpcBaseKeys::CurrentState; }
	
	UFUNCTION(BlueprintPure, meta = (ToolTip = "CanAttack"))
	static FName GetCanAttack() {  return NpcBaseKeys::bCanAttack; }

	UFUNCTION(BlueprintPure, meta = (ToolTip = "CanRotateWhileAttack"))
	static FName GetCanRotateWhileAttack() {  return NpcBaseKeys::bCanRotateWhileAttack; }

	UFUNCTION(BlueprintPure, meta = (ToolTip = "CurrentDoubtState"))
	static FName GetCurrentDoubtState() {  return NpcBaseKeys::CurrentDoubtState; }
	
	UFUNCTION(BlueprintPure, meta = (ToolTip = "bMoveReverse"))
	static FName GetMoveReverseKey() {  return NpcBaseKeys::PatrolKeys::bMoveReverse; }

	UFUNCTION(BlueprintPure, meta = (ToolTip = "PatrolType"))
	static FName GetPatrolTypeKey() {  return NpcBaseKeys::PatrolKeys::PatrolType; }

};
