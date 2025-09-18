// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackboardKeyLibrary.h"

namespace NpcBaseKeys
{
	const FName TargetActor = FName("TargetActor");
	const FName TargetLocation = FName("TargetLocation");
	const FName TargetDir = FName("TargetDir");
	const FName ReturnLocation = FName("ReturnLocation");
	const FName CurrentState = FName("CurrentState");
	const FName bCanAttack = FName("bCanAttack");
	const FName bCanRotateWhileAttack = FName("bCanRotateWhileAttack");
	const FName CurrentDoubtState = FName("CurrentDoubtState");
	
	namespace PatrolKeys
	{
		FName bMoveReverse = FName("bMoveReverse");
		FName PatrolType = FName("PatrolType");
	}
}