#include "GlobalUtilsLibrary.h"
#include "Algo/Transform.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"

#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/CoreGameInstance.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateStructLocalization.h"
#include "ProjectT/System/Core/Settings/GameSettings/CoreGameUserSettings.h"


bool UGlobalUtilsLibrary::IsValueInRange(const int32 Value, const int32 A, const int32 B, bool GreaterEqual, bool LessEqual)
{
	bool G = GreaterEqual ? (Value >= A) : (Value > A);
	bool L = LessEqual ? (Value <= B) : (Value < B);
	return G && L;
}

UWorld* UGlobalUtilsLibrary::GetValidWorld(UObject* Object)
{
	UWorld* World = Object->GetWorld();
	NMT_CHECKF(World);
	return World;
}

UDataDam* UGlobalUtilsLibrary::GetSharedDataDam(UObject* Object)
{
	NMT_CHECKF(Object);

	UWorld* World = GetValidWorld(Object);
	NMT_CHECKF(World);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
	NMT_CHECKF(GI);

	UDataDam* Ret = IGlobalObjectProvider::Execute_GetDataDam(GI);
	NMT_CHECKF(Ret);

	return Ret;
}

UStageManager* UGlobalUtilsLibrary::GetSharedLevelManager(UObject* Object)
{
	NMT_CHECKF(Object);

	UWorld* World = GetValidWorld(Object);
	NMT_CHECKF(World);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
	NMT_CHECKF(GI);

	UStageManager* Ret = IGlobalObjectProvider::Execute_GetStageManager(GI);
	NMT_CHECKF(Ret);

	return Ret;
}

UOptionManager* UGlobalUtilsLibrary::GetSharedOptionManager(UObject* Object)
{
	NMT_CHECKF(Object);

	UWorld* World = GetValidWorld(Object);
	NMT_CHECKF(World);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
	NMT_CHECKF(GI);

	UOptionManager* Ret = IGlobalObjectProvider::Execute_GetOptionManager(GI);
	NMT_CHECKF(Ret);

	return Ret;
}

USoundManager* UGlobalUtilsLibrary::GetSharedSoundManager(UObject* Object)
{
	NMT_CHECKF(Object);

	UWorld* World = GetValidWorld(Object);
	NMT_CHECKF(World);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
	NMT_CHECKF(GI);

	USoundManager* Ret = IGlobalObjectProvider::Execute_GetSoundManager(GI);
	NMT_CHECKF(Ret);

	return Ret;
}

UCoreCheatManager* UGlobalUtilsLibrary::GetSharedCoreCheatManager(UObject* Object)
{
	NMT_CHECKF(Object);

	UWorld* World = GetValidWorld(Object);
	NMT_CHECKF(World);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
	NMT_CHECKF(GI);

	UCoreCheatManager* Ret = IGlobalObjectProvider::Execute_GetCoreCheatManager(GI);
	NMT_CHECKF(Ret);

	return Ret;
}

UNotificationCenter* UGlobalUtilsLibrary::GetSharedNotificationCenter(UObject* Object)
{
	NMT_CHECKF(Object);

	UWorld* World = GetValidWorld(Object);
	NMT_CHECKF(World);

	UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
	NMT_CHECKF(GI);

	UNotificationCenter* Ret = IGlobalObjectProvider::Execute_GetNotificationCenter(GI);
	NMT_CHECKF(Ret);

	return Ret;
}

bool UGlobalUtilsLibrary::IsStandalonePlay(UObject* Object)
{
	NMT_CHECKF(Object);
	ENetMode NetMode = Object->GetWorld()->GetNetMode();
	return NetMode == NM_Standalone;
}

TArray<FString> UGlobalUtilsLibrary::ParseStringIndices(const TCHAR* Delimiter, const FString& Indices)
{
	TArray<FString> Ret;
	Indices.ParseIntoArray(Ret, TEXT(";"));
	return Ret;
}

FString UGlobalUtilsLibrary::GetScriptTypeByRefPath(const FString& RefPath)
{
	FString Ret;
	if (RefPath.Split(TEXT("."), nullptr, &Ret))
		Ret.Split(TEXT("'"), &Ret, nullptr);

	return Ret;
}

EContentType UGlobalUtilsLibrary::GetContentTypeFromIndex(FName InRowName)
{
	FString RowString = InRowName.ToString();
	FString SliceString = DivideString(2, RowString);
	int32 ContentTypeNumber = FCString::Atoi(*SliceString);
	return StaticCast<EContentType>(ContentTypeNumber);
}

TArray<FString> UGlobalUtilsLibrary::ParseStringIndices(const FString& Indices, const FString& Delimeter)
{
	TArray<FString> Ret;
	Indices.ParseIntoArray(Ret, *Delimeter);
	return Ret;
}

TArray<AActor*> UGlobalUtilsLibrary::GetFilteredCurrentActors(const TArray<AActor*>& InActors,
	const TArray<UClass*>& IgnoreClasses, const TArray<UClass*>& IncludeClasses)
{
	bool bActorSubclass = false;
	bool bInterfaceClass = false;
	for (UClass* Class : IgnoreClasses)
	{
		bActorSubclass = Class->IsChildOf(AActor::StaticClass());
		bInterfaceClass = Class->IsChildOf(UInterface::StaticClass());
		NMT_MSG_CHECKF(bActorSubclass || bInterfaceClass, "%s Contains a class that isn’t allowed: %s", TEXT("IgnoreClasses"), *Class->GetName());
	}

	for (UClass* Class : IncludeClasses)
	{
		bActorSubclass = Class->IsChildOf(AActor::StaticClass());
		bInterfaceClass = Class->IsChildOf(UInterface::StaticClass());
		NMT_MSG_CHECKF(bActorSubclass || bInterfaceClass, "%s Contains a class that isn’t allowed: %s", TEXT("IncludeClasses"), *Class->GetName());
	}

	TArray<TArray<AActor*>::ElementType> Ret = InActors.FilterByPredicate([&IgnoreClasses, &IncludeClasses](const AActor* Actor)
	{
		UClass* CurrentActorClass = Actor->GetClass();
		if(IgnoreClasses.Contains(CurrentActorClass)) return false;
		
		for(UClass* Class : IgnoreClasses)
		{
			if(Actor->IsA(Class)) return false;
			if(CurrentActorClass->ImplementsInterface(Class)) return false;
		}

		if(IncludeClasses.Num() == 0) return true;
		if(IncludeClasses.Contains(CurrentActorClass)) return true;

		for(UClass* Class : IncludeClasses)
		{
			if(Actor->IsA(Class)) return true;
			if(CurrentActorClass->ImplementsInterface(Class)) return true;
		}
		return false;
	});
	
	return Ret;
}

FString UGlobalUtilsLibrary::ReferencePathToFilePath(const FString& ReferencePath)
{
	FString Ret, Str;
	ReferencePath.Split("\'", nullptr, &Str);
	Str.Split(".", &Ret, nullptr);
	return Ret;
}

FString UGlobalUtilsLibrary::Uint32ToBase64String(uint32 Value)
{
	const FString Alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@";
	FString Result = "";
	if (Value == 0) Result = Alphabet.Mid(0, 1);
	for (int i = 1; i < 6; i++)
	{
		uint32 Index = Value % Alphabet.Len();
		Result = Alphabet.Mid(Index, 1) + Result;
		Value = Value / Alphabet.Len();
	}
	return Result;
}

TSoftClassPtr<UUserWidget> UGlobalUtilsLibrary::GetSoftWidgetClassFromPath(FString Path)
{
	return GetStringToSoftClass<UUserWidget>(Path);
}

bool UGlobalUtilsLibrary::IsEditorPlay()
{
#if WITH_EDITOR
	return true;

#else
	return false;

#endif
}

void UGlobalUtilsLibrary::EnableComponent(UActorComponent* Component, bool bEnable)
{
	NMT_CHECKF(Component);

	Component->SetComponentTickEnabled(bEnable);
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component))
	{
		PrimitiveComponent->SetVisibility(bEnable, true);
		PrimitiveComponent->SetHiddenInGame(!bEnable, true);
		PrimitiveComponent->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}

FString UGlobalUtilsLibrary::GetLocalizedStringFromTable(const FName& TableRow)
{	
	UWorld* World = GEngine->GameViewport->GetWorld();
	if (!World)	return FString();

	UDataDam* DD = GetSharedDataDam(World);
	NMT_CHECKF(DD);

	const UDataTable* LocalizeDT = DD->GetDataTableByEnum(EDataTableKey::Localization);	
	FLocalization* LocalizeST = LocalizeDT->FindRow<FLocalization>(TableRow, LocalizeDT->GetName());
	if (!LocalizeST) return FString();

	UCoreGameUserSettings* UserSetting = UCoreGameUserSettings::GetCoreGameUserSettings();
	EGameLanguage CurLanguage = UserSetting->GetGamePlayOption().Setting_Language;

	FString OutputString = FString();

	switch (CurLanguage)
	{
	    case EGameLanguage::Korea:
    	{
	    	OutputString = LocalizeST->sKorean;
	    	break;
	    }
	    case EGameLanguage::English:
	    {
	    	OutputString = LocalizeST->sEnglish;
	    	break;
    	}
    	default:
	    {
	    	OutputString = LocalizeST->sEnglish;
	    	break;
    	}
	}
	return OutputString;
}

/*NOTE:
베지에 곡선의 드 카스텔조 알고리즘을 사용하여 주어진 제어점 Points에 대한 시간 t에서의 보정위치를 구한다.
Points 배열의 첫번째 인덱스에는	시작점, 마지막 인덱스에는 끝점이 위치해야 한다.
*/
FVector UGlobalUtilsLibrary::GetBezierPointByDeCasteljau(const TArray<FVector>& Points, float t)
{
	if(Points.Num() == 1)
		return Points[0];

	TArray<FVector> NextLevel;
	for(int i = 0; i < Points.Num() - 1; ++i)
	{
		FVector LerpPoint = FMath::Lerp(Points[i], Points[i + 1], t);
		NextLevel.Add(LerpPoint);
	}

	return GetBezierPointByDeCasteljau(NextLevel, t);
}

int32 UGlobalUtilsLibrary::GetEnumIndexFromString(EDataTableKey InEnum, FString InString)
{
	int32 Ret = 0;
	FString FindEnumType = "E" + EnumToString(InEnum);
	UEnum* EnumPtr = FindFirstObjectSafe<UEnum>(*FindEnumType);
	if (EnumPtr == nullptr) return Ret;
	{
		Ret = EnumPtr->GetIndexByNameString(InString);
		return Ret;
	}
}

FVector UGlobalUtilsLibrary::GetCenterPointBetweenTwoVectors(FVector A, FVector B)
{
	return (A + B) * 0.5f;
}

double UGlobalUtilsLibrary::FindAngleBetweenTwoVectors2D(FVector2D A, FVector2D B)
{
	A.Normalize();
	B.Normalize();

	double Scalar = A | B;
	double Angle = UKismetMathLibrary::DegAcos(Scalar);
	Angle = (A ^ B) > 0 ? Angle : Angle * -1.0;
	return Angle;
}

double UGlobalUtilsLibrary::FindAngleBetweenTwoVectors3D(FVector A, FVector B, EFindAngleAxis EFindAxis)
{
	A.Normalize();
	B.Normalize();

	double Scalar = A | B;
	double Angle = UKismetMathLibrary::DegAcos(Scalar);
	FVector CrossVector = A ^ B;
	switch (EFindAxis)
	{
	case EFindAngleAxis::EForward:
		Angle = CrossVector.X > 0 ? Angle : Angle * -1.0;
		break;
	case EFindAngleAxis::EUp:
		Angle = CrossVector.Z > 0 ? Angle : Angle * -1.0;
		break;
	case EFindAngleAxis::ERight:
		Angle = CrossVector.Y > 0 ? Angle : Angle * -1.0;
		break;
	}
	return Angle;
}

bool UGlobalUtilsLibrary::SuggestProjectileVelocityAtLocation(const UObject* WorldContextObject,
	FVector& OutLaunchVelocity, const FVector& ProjectileStartLocation, const FVector& TargetLocation, const float DesiredArcHeight, double GravityZOverride,
	double TimeToTarget)
{
	if (!GEngine) return false;
	if (TargetLocation == FVector::ZeroVector)	return false;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)	return false;

	const double Gravity = FMath::IsNearlyZero(GravityZOverride) ? World->GetGravityZ() : GravityZOverride;
	const double StartZ = ProjectileStartLocation.Z;
	const double TargetZ = TargetLocation.Z;
	const double ApexZ = TargetZ + DesiredArcHeight;
	if (ApexZ <= StartZ || ApexZ <= TargetZ)	return false;

	const double Sqrt = FMath::Sqrt(2.0 * FMath::Abs(Gravity) * (ApexZ - StartZ));
	const double Up = Sqrt / FMath::Abs(Gravity);
	const double Down = FMath::Sqrt(2.0 * (ApexZ - TargetZ) / FMath::Abs(Gravity));
	const double Time = Up + Down;
	const FVector Delta = TargetLocation - ProjectileStartLocation;
	const FVector DeltaXY = FVector(Delta.X, Delta.Y, 0.0);
	const FVector VelXY = DeltaXY / Time;
	OutLaunchVelocity = FVector(VelXY.X, VelXY.Y, Sqrt);

	return true;
}

// EX: (X=0.0000,Y=0.0000,Z=0.0000) -> FVector3D
FVector UGlobalUtilsLibrary::StringToVector3D(FString& InString)
{
	FString S = InString.Mid(1, InString.Len() - 2);
	TArray<FString> ParseString;
	S.ParseIntoArray(ParseString, TEXT(","));
	NMT_MSG_CHECKF(ParseString.Num() == 3, "String Vector Has Fewer Than Three Elements");

	float X = FCString::Atod(*ParseString[0] + 2);
	float Y = FCString::Atod(*ParseString[1] + 2);
	float Z = FCString::Atod(*ParseString[2] + 2);
	FVector Ret = FVector(X, Y, Z);

	return Ret;
}

FString UGlobalUtilsLibrary::DivideString(const int Count, FString& InString)
{
	if (InString.Len() < Count) return FString();
	FString Ret = InString.Left(Count);
	InString.RemoveAt(0, Count);
	return Ret;
}

void UGlobalUtilsLibrary::DebugBreak(const FString& MessageString)
{
	checkf(0, TEXT("%s"), *MessageString);
}
