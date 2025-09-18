// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreCheatManager.h"

#include "DrawDebugHelpers.h"
#include "QuestManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CapsuleComponent.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Characters/NPC/CoreNpc.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Components/CoreCameraComponent.h"
#include "ProjectT/System/Core/GameModes/WorldGameMode.h"
#include "ProjectT/System/Core/GameModes/CoreGameState.h"
#include "ProjectT/System/SinglePlay/GameModes/SPGameModeBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Settings/GeneralSettings/CoreGameGeneralSettings.h"
#include "ProjectT/System/Core/Objects/LevelObject/CeilObject.h"
#include "ProjectT/System/SinglePlay/Characters/NPC/SPNpcAgent.h"


UCoreCheatManager::UCoreCheatManager() :
	OuterObject(nullptr),
	DebugWidgetVisibility(ESlateVisibility::Visible),
	bDebugToggleCheat(0),
	bInitialized(0)
{
}

void UCoreCheatManager::BroadCastDebugStatus()
{
	UScriptStruct* DebugStatusStruct = DebugStatus.StaticStruct();
	for(TFieldIterator<FProperty> It = DebugStatusStruct; It; ++It)
	{
		FProperty* Property = *It;
		NMT_CHECKF(Property);

		const FName PropName = *Property->GetName();
		bool State = UGlobalUtilsLibrary::GetBoolFieldInStruct<FDebugStatus>(&DebugStatus, PropName);
		if(State)
		{
			uint16 ParamSize = CheatFunctionMap[PropName]->ParmsSize;
			void* Params = FMemory_Alloca(ParamSize);
			if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(PropName, State);
		}
	}
}

void UCoreCheatManager::DebugRestartGame() const
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);

	AGameModeBase* GM = UGameplayStatics::GetGameMode(OuterObject);
	NMT_CHECKF(GM);

	AWorldGameMode* CGM = Cast<AWorldGameMode>(GM);
	NMT_CHECKF(CGM);

	CGM->UnPauseGame();

	FString CLN = UGameplayStatics::GetCurrentLevelName(OuterObject);
	UGameplayStatics::OpenLevel(OuterObject, FName(CLN));
}

void UCoreCheatManager::DebugDrawFrustum() const
{
	UWorld* World = OuterObject->GetWorld();
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(OuterObject);
	ACorePlayerCharacter* Player = UGlobalUtilsLibrary::GetLocalPlayerUnChecked<ACorePlayerCharacter>(OuterObject);
	if(!NMT_ENSURE(Player)) return;

	FMinimalViewInfo ViewInfo;
	TObjectPtr<UCoreCameraComponent> LocCoreCamera;
	Player->GetCameraComponent(LocCoreCamera);
	LocCoreCamera->GetCameraView(DeltaTime, ViewInfo);

	FColor Color = FColor::Magenta;
	float LifeTime = 0.f;
	float Thickness = 1.f;
	float Near = 10.f;
	float Far = 1000.f;

	const FVector Forward = ViewInfo.Rotation.Vector();
	const FVector Right = FRotationMatrix(ViewInfo.Rotation).GetScaledAxis(EAxis::Y);
	const FVector Up = FRotationMatrix(ViewInfo.Rotation).GetScaledAxis(EAxis::Z);

	const float HalfFOV = FMath::DegreesToRadians(ViewInfo.FOV * 0.5f);
	const float HNear = Near * FMath::Tan(HalfFOV);
	const float WNear = HNear * ViewInfo.AspectRatio;
	const float HFar = Far * FMath::Tan(HalfFOV);
	const float WFar = HFar * ViewInfo.AspectRatio;

	const FVector CNear = ViewInfo.Location + Forward * Near;
	const FVector CFar = ViewInfo.Location + Forward * Far;

	FVector NTL = CNear + Up * HNear - Right * WNear;
	FVector NTR = CNear + Up * HNear + Right * WNear;
	FVector NBL = CNear - Up * HNear - Right * WNear;
	FVector NBR = CNear - Up * HNear + Right * WNear;

	FVector FTL = CFar + Up * HFar - Right * WFar;
	FVector FTR = CFar + Up * HFar + Right * WFar;
	FVector FBL = CFar - Up * HFar - Right * WFar;
	FVector FBR = CFar - Up * HFar + Right * WFar;

	DrawDebugLine(World, NTL, NTR, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, NTR, NBR, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, NBR, NBL, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, NBL, NTL, Color, false, LifeTime, 0, Thickness);

	DrawDebugLine(World, FTL, FTR, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, FTR, FBR, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, FBR, FBL, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, FBL, FTL, Color, false, LifeTime, 0, Thickness);

	DrawDebugLine(World, NTL, FTL, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, NTR, FTR, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, NBR, FBR, Color, false, LifeTime, 0, Thickness);
	DrawDebugLine(World, NBL, FBL, Color, false, LifeTime, 0, Thickness);
}

void UCoreCheatManager::BindOnDebugSPGame(const FOnDebugSPGame& Callback)
{
	if(!OnDebugSPGame.IsBound()) OnDebugSPGame = Callback;
}

void UCoreCheatManager::RefreshStatusEvent()
{
	UScriptStruct* DebugStatusStruct = DebugStatus.StaticStruct();
	for(TFieldIterator<FProperty> It = DebugStatusStruct; It; ++It)
	{
		FProperty* Property = *It;
		NMT_CHECKF(Property);

		const FName PropName = *Property->GetName();
		bool FieldState = UGlobalUtilsLibrary::GetBoolFieldInStruct<FDebugStatus>(&DebugStatus, PropName);
		if(FieldState)
		{
			uint16 ParamSize = CheatFunctionMap[PropName]->ParmsSize;
			void* Params = FMemory_Alloca(ParamSize);
			ProcessEvent(CheatFunctionMap[PropName], Params);
			ProcessEvent(CheatFunctionMap[PropName], Params);
		}
	}
}

void UCoreCheatManager::DebugToggleCheat()
{
	if(!DebugWidget)
	{
		const UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
		NMT_CHECKF(GI);

		FString ClassPath = UAssetUtilsLibrary::AssetReferencePathToClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Debug/WBP_DebugMainPanel.WBP_DebugMainPanel'"));
		UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);
		UWorld* World = OuterObject->GetWorld();
		DebugWidget = CreateWidget(World, LoadedClass);
		DebugWidget->AddToViewport(1);
		DebugWidgetVisibility = DebugWidget->GetVisibility();
	}
	else
	{
		if(!DebugWidget->IsInViewport()) DebugWidget->AddToViewport(1);
		else if(DebugWidget->GetVisibility() != ESlateVisibility::Collapsed) DebugWidget->SetVisibility(ESlateVisibility::Collapsed);
		else DebugWidget->SetVisibility(DebugWidgetVisibility);
	}
	bDebugToggleCheat = !bDebugToggleCheat;
}

void UCoreCheatManager::PropagateStatusEvent(const bool State)
{
	UScriptStruct* DebugStatusStruct = DebugStatus.StaticStruct();
	for(TFieldIterator<FProperty> It = DebugStatusStruct; It; ++It)
	{
		FProperty* Property = *It;
		NMT_CHECKF(Property);

		const FName PropName = *Property->GetName();
		bool FieldState = UGlobalUtilsLibrary::GetBoolFieldInStruct<FDebugStatus>(&DebugStatus, PropName);
		if(FieldState == State)
		{
			uint16 ParamSize = CheatFunctionMap[PropName]->ParmsSize;
			void* Params = FMemory_Alloca(ParamSize);
			ProcessEvent(CheatFunctionMap[PropName], Params);
		}
	}
}

void UCoreCheatManager::DebugResetStatus()
{
	PropagateStatusEvent(true);
	SaveDebugStatus = DebugStatus;
}

void UCoreCheatManager::DebugTogglePlayerInvincibleHP()
{
	DebugStatus.bDebugPlayerInvincibleHP = !DebugStatus.bDebugPlayerInvincibleHP;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugPlayerInvincibleHP), DebugStatus.bDebugPlayerInvincibleHP);
}

void UCoreCheatManager::DebugTogglePlayerDamage()
{
	DebugStatus.bDebugPlayerDamage = !DebugStatus.bDebugPlayerDamage;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugPlayerDamage), DebugStatus.bDebugPlayerDamage);
}

void UCoreCheatManager::DebugTogglePlayerSpeed()
{
	DebugStatus.bDebugPlayerSpeed = !DebugStatus.bDebugPlayerSpeed;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugPlayerSpeed), DebugStatus.bDebugPlayerSpeed);
}

void UCoreCheatManager::DebugTogglePlayerInvincibleStatusEffect()
{
	DebugStatus.bDebugPlayerInvincibleStatusEffect = !DebugStatus.bDebugPlayerInvincibleStatusEffect;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugPlayerInvincibleStatusEffect), DebugStatus.bDebugPlayerInvincibleStatusEffect);
}

void UCoreCheatManager::DebugTogglePlayerInvincibleSkill()
{
	DebugStatus.bDebugPlayerInvincibleSkill = !DebugStatus.bDebugPlayerInvincibleSkill;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugPlayerInvincibleSkill), DebugStatus.bDebugPlayerInvincibleSkill);
}

void UCoreCheatManager::DebugToggleEnemyInvincibleHP()
{
	DebugStatus.bDebugEnemyInvincibleHP = !DebugStatus.bDebugEnemyInvincibleHP;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugEnemyInvincibleHP), DebugStatus.bDebugEnemyInvincibleHP);
}

void UCoreCheatManager::DebugToggleEnemyDamage()
{
	DebugStatus.bDebugEnemyDamage = !DebugStatus.bDebugEnemyDamage;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugEnemyDamage), DebugStatus.bDebugEnemyDamage);
}

void UCoreCheatManager::DebugToggleEnemySpeed()
{
	DebugStatus.bDebugEnemySpeed = !DebugStatus.bDebugEnemySpeed;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugEnemySpeed), DebugStatus.bDebugEnemySpeed);
}

void UCoreCheatManager::DebugToggleEnemyInvincibleStatusEffect()
{
	DebugStatus.bDebugEnemyInvincibleStatusEffect = !DebugStatus.bDebugEnemyInvincibleStatusEffect;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugEnemyInvincibleStatusEffect), DebugStatus.bDebugEnemyInvincibleStatusEffect);
}

void UCoreCheatManager::DebugToggleEnemyInvincibleSkill()
{
	DebugStatus.bDebugEnemyInvincibleSkill = !DebugStatus.bDebugEnemyInvincibleSkill;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugEnemyInvincibleSkill), DebugStatus.bDebugEnemyInvincibleSkill);
}

void UCoreCheatManager::DebugToggleVision()
{
	DebugStatus.bDebugVision = !DebugStatus.bDebugVision;

	UWorld* World = OuterObject->GetWorld();
	NMT_CHECKF(World);

	TArray<AActor*> Npcs;
	UGameplayStatics::GetAllActorsOfClass(World, ACoreNpc::StaticClass(), Npcs);
	for(AActor* Npc : Npcs)
	{
		if(ACoreNpc* CastToNpc = Cast<ACoreNpc>(Npc)) CastToNpc->DebugToggleFrustum();
	}

	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugVision), DebugStatus.bDebugVision);
}

void UCoreCheatManager::DebugToggleHitCollision()
{
	DebugStatus.bDebugHitCollision = !DebugStatus.bDebugHitCollision;

	UWorld* World = OuterObject->GetWorld();
	NMT_CHECKF(World);

	ACorePlayerCharacter* Player = UGlobalUtilsLibrary::GetLocalPlayerUnChecked<ACorePlayerCharacter>(OuterObject);
	NMT_CHECKF(Player);

	Player->GetCapsuleComponent()->SetHiddenInGame(!DebugStatus.bDebugHitCollision);

	TArray<AActor*> Npcs;
	UGameplayStatics::GetAllActorsOfClass(World, ACoreNpc::StaticClass(), Npcs);

	for(AActor* Npc : Npcs)
	{
		if(ACoreNpc* CastToNpc = Cast<ACoreNpc>(Npc))
		{
			if(ASPNpcAgent* NpcAgent = Cast<ASPNpcAgent>(CastToNpc))
			{
			}
			else CastToNpc->GetCapsuleComponent()->SetHiddenInGame(!DebugStatus.bDebugHitCollision);
		}
	}

	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugHitCollision), DebugStatus.bDebugHitCollision);
}

void UCoreCheatManager::DebugToggleTimeStop()
{
	DebugStatus.bDebugTimeStop = !DebugStatus.bDebugTimeStop;

	UWorld* World = OuterObject->GetWorld();
	NMT_CHECKF(World);

	/**
	 * NOTE: 현재는 SinglePlay만 적용됨
	 */
	AGameModeBase* GM = UGameplayStatics::GetGameMode(World);
	if(ASPGameModeBase* SPGM = Cast<ASPGameModeBase>(GM))
	{
		SPGM->DebugGameTimePause(DebugStatus.bDebugTimeStop);
	}

	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugTimeStop), DebugStatus.bDebugTimeStop);
}

void UCoreCheatManager::DebugToggleSlomo()
{
	const FString SpeedCommand = !DebugStatus.bDebugSlomo ? TEXT("Slomo 10") : TEXT("Slomo 1");
	UKismetSystemLibrary::ExecuteConsoleCommand(OuterObject, SpeedCommand);
	DebugStatus.bDebugSlomo = !DebugStatus.bDebugSlomo;
	if(OnDebugSPGame.IsBound()) OnDebugSPGame.Execute(GET_MEMBER_NAME_CHECKED(FDebugStatus, bDebugSlomo), DebugStatus.bDebugSlomo);
}

void UCoreCheatManager::DebugSpectatorPlay()
{
	UWorld* World = OuterObject->GetWorld();
	ACorePlayerCharacter* Player = UGlobalUtilsLibrary::GetLocalPlayerUnChecked<ACorePlayerCharacter>(OuterObject);
	if(!NMT_ENSURE(Player)) return;
	TObjectPtr<UCoreCameraComponent> CoreCam;
	Player->GetCameraComponent(CoreCam);
	FTransform CamTransform = CoreCam->GetComponentTransform();
	TArray<AActor*> FindCeil;
	UGameplayStatics::GetAllActorsOfClass(World, ACeilObject::StaticClass(), FindCeil);

	for(AActor* Ceil : FindCeil)
	{
		Ceil->SetActorHiddenInGame(true);
	}

	DebugToggleTimeStop();
	if(AActor* FindSpectatorPawn = UGameplayStatics::GetActorOfClass(World, ASpectatorPawn::StaticClass()))
	{
		FindSpectatorPawn->SetActorTransform(CamTransform);
		UGameplayStatics::GetPlayerController(World, 0)->SetViewTarget(FindSpectatorPawn);
		UGameplayStatics::GetPlayerController(World, 0)->Possess(Cast<ASpectatorPawn>(FindSpectatorPawn));
		return;
	}
	ASpectatorPawn* SpectatorPawn = World->SpawnActor<ASpectatorPawn>(UCoreGameGeneralSettings::GetDebugSpectatorPawn(), CamTransform);
	UGameplayStatics::GetPlayerController(World, 0)->SetViewTarget(SpectatorPawn);
	UGameplayStatics::GetPlayerController(World, 0)->Possess(SpectatorPawn);
}

void UCoreCheatManager::DebugAddMainQuest()
{
	UWorld* World = OuterObject->GetWorld();
	NMT_CHECKF(World);

	AGameModeBase* GM = UGameplayStatics::GetGameMode(World);
	NMT_CHECKF(GM);

	UQuestManager* QM = IWorldObjectProvider::Execute_GetQuestManager(GM);
	NMT_CHECKF(QM);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);

	QM->CheckObjectiveSatisfied(QM->MissionData.GetObjectiveSubType(), QM->MissionData.GetObjectiveIndex());
}

void UCoreCheatManager::DebugAddSubQuest(const int32 QuestIndex)
{
	UWorld* World = OuterObject->GetWorld();
	NMT_CHECKF(World);

	AGameModeBase* GM = UGameplayStatics::GetGameMode(World);
	NMT_CHECKF(GM);

	UQuestManager* QM = IWorldObjectProvider::Execute_GetQuestManager(GM);
	NMT_CHECKF(QM);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);

	if(QM->SubQuestList.IsValidIndex(QuestIndex))
	{
		QM->CheckObjectiveSatisfied(QM->SubQuestList[QuestIndex].GetObjectiveSubType(), QM->SubQuestList[QuestIndex].GetObjectiveIndex());
	}
}

FDebugStatus UCoreCheatManager::GetDebugStatus()
{
	return DebugStatus;
}

void UCoreCheatManager::InitializeManager(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);

	UScriptStruct* DebugStatusStruct = GetDebugStatus().StaticStruct();
	for(TFieldIterator<FProperty> It = DebugStatusStruct; It; ++It)
	{
		FProperty* Property = *It;
		NMT_CHECKF(Property);
		const FName PropName = *Property->GetName();
		CheatFunctionMap.FindOrAdd(PropName, nullptr);
	}

	for(auto Iter = CheatFunctionMap.CreateIterator(); Iter; ++Iter)
	{
		for(TFieldIterator<UFunction> It = GetClass(); It; ++It)
		{
			UFunction* Func = *It;
			const FString FuncName = Func->GetName();
			const FName KeyName = Iter->Key;
			FString FormatKeyName = KeyName.ToString();

			FString PrefixName = TEXT("bDebug");
			FormatKeyName.RemoveFromStart(PrefixName);

			if(FuncName.Contains(FormatKeyName)) CheatFunctionMap[KeyName] = Func;
		}
	}

	if(bInitialized) RefreshStatusEvent();
	else bInitialized = true;
}

void UCoreCheatManager::ReleaseManager()
{
	OuterObject = nullptr;
}
