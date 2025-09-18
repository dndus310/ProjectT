// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "ProjectT/Data/Gen/GenerateEnumContentType.h"
#include "GlobalUtilsLibrary.generated.h"

struct FNActionData;
enum class EDataTableKey : uint8;

UENUM(BlueprintType)
enum class EFindAngleAxis :uint8
{
	EForward UMETA(DisplayName="EForward"),
	EUp UMETA(DisplayName="EUp"),
	ERight UMETA(DisplayName="ERight")
};

UCLASS()
class PROJECTT_API UGlobalUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UWorld* GetValidWorld(UObject* Object);
	static class UDataDam* GetSharedDataDam(UObject* Object);
	static class UStageManager* GetSharedLevelManager(UObject* Object);
	static class UOptionManager* GetSharedOptionManager(UObject* Object);
	static class USoundManager* GetSharedSoundManager(UObject* Object);	
	static class UCoreCheatManager* GetSharedCoreCheatManager(UObject* Object);
	static class UNotificationCenter* GetSharedNotificationCenter(UObject* Object);
	static bool IsStandalonePlay(UObject* Object);
	
	static TArray<FString> ParseStringIndices(const TCHAR* Delimiter, const FString& Indices);
	static TArray<FString> ParseStringIndices(const FString& Indices, const FString& Delimeter);
	
	UFUNCTION(BlueprintCallable)
	static FString DivideString(const int Count, FString& InString);

	static int32 GetEnumIndexFromString(EDataTableKey InEnum, FString InString);
	static FString Uint32ToBase64String(uint32 Value);
	
	template <typename EnumType>
	static FString GetEnumToString(EnumType FindEnumData)
	{
		const UEnum* FindPtr = StaticEnum<EnumType>();
		if (!FindPtr) return FString();

		const int32 Idx = static_cast<int32>(FindEnumData);
		if (Idx == 0) return FString();

		FString Ret = FindPtr->GetNameStringByIndex(Idx);
		return Ret;
	}
	
	template <typename EnumType>
	static EnumType GetStringToEnum(const FString& FindEnumTypeString)
	{
		EnumType Ret = StaticCast<EnumType>(0);
		for (EnumType e : TEnumRange<EnumType>())
		{
			FString Name = UGlobalUtilsLibrary::GetEnumToString<EnumType>(e);
			if (FindEnumTypeString.Equals(Name))
				return e;
		}
		return Ret;
	}
	
	template<typename T>
	static TSoftClassPtr<T> GetStringToSoftClass(FString& InString)
	{
		FSoftClassPath SoftClassPath(InString.Append("_C"));
		return TSoftClassPtr<T>(SoftClassPath);
	}
	
	template <typename T>
	static void AsyncLoadClassAsset(FString AssetPath, TFunction<void(T*)> Callback)
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		FSoftObjectPath AssetRef(AssetPath);
		Streamable.RequestAsyncLoad(AssetRef,[AssetRef, Callback]()
		{
			T* LoadedAsset = Cast<T>(AssetRef.TryLoad());
			LoadedAsset = LoadedAsset ? LoadedAsset : nullptr; 	
			Callback(LoadedAsset);
		});
	}
		
	template <typename T>
	static void AsyncLoadSubClass(TSoftClassPtr<T> SoftClassPtr, TFunction<void(TSubclassOf<T>)> Callback)
	{
		if (SoftClassPtr.IsValid())
		{
			Callback(SoftClassPtr.Get());
			return;
		}

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(SoftClassPtr.ToSoftObjectPath(), [SoftClassPtr, Callback]()
			{
				TSubclassOf<T> LoadedClass = SoftClassPtr.Get();
				Callback(LoadedClass);
			}
		);
	}
	
	template <typename T>
	static T* FindObject(const FString& FilePath)
	{
		T* Object = ConstructorHelpers::FObjectFinder<T>(*FilePath).Object;
		NMT_ENSURE(Object);
		
		return Object;
	}
	
	template <typename T>
	static ConstructorHelpers::FClassFinder<T> FindClass(const FString& FilePath)
	{
		ConstructorHelpers::FClassFinder<T> ClassFinder(*FilePath);
		NMT_ENSURE(ClassFinder.Succeeded());
		
		return ClassFinder;
	}

	template <typename T>
	static T* FindActorInLevel(ULevelStreaming* Level)
	{
		if(!NMT_ENSURE(Level && Level->IsLevelLoaded())) return nullptr;
		ULevel* LV = Level->GetLoadedLevel();

		if(!NMT_ENSURE(LV)) return nullptr;
		for(AActor* Actor : LV->Actors)
			if(T* Ret = Cast<T>(Actor)) return Ret;

		return nullptr;
	}

	template <typename T>
	static T* GetGameModeChecked(const UObject* WorldContextObject)
	{
		return CastChecked<T>(UGameplayStatics::GetGameMode(WorldContextObject));
	}

	template <typename T>
	static T* GetGameStateChecked(const UObject* WorldContextObject)
	{
		return CastChecked<T>(UGameplayStatics::GetGameState(WorldContextObject));
	}

	template <typename T>
	static T* GetLocalPlayerChecked(const UObject* WorldContextObject)
	{
		return CastChecked<T>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0));
	}

	template <typename T>
	static T* GetLocalPlayerUnChecked(const UObject* WorldContextObject)
	{
		return Cast<T>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0));
	}

	template <typename T>
	static T* GetLocalPlayerControllerChecked(const UObject* WorldContextObject)
	{
		return CastChecked<T>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	}

	template <typename T>
	static T* GetLocalPlayerControllerUnChecked(const UObject* WorldContextObject)
	{
		return Cast<T>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	}

	template<typename T>
	static void ShuffleArray(const TArray<T>& TargetArray)
	{
		int32 LastIndex = TargetArray.Num() - 1;
		for (int32 i = 0; i < LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				const_cast<TArray<T>*>(&TargetArray)->Swap(i, Index);
			}
		}
	}
	
	static FString GetScriptTypeByRefPath(const FString& RefPath);
	static EContentType GetContentTypeFromIndex(FName InRowName);

	template <typename T>
	static FString GetStringFieldInStruct(T* StructInstance, const FName& FieldName)
	{
		UStruct* StructType = StructInstance->StaticStruct();
		FProperty* Prop = StructType->FindPropertyByName(FieldName);
		const FStrProperty* PropType = CastField<FStrProperty>(Prop);
		FString Ret = PropType->GetPropertyValue_InContainer(StructInstance);
		return Ret;	
	}

	template <typename T>
	static float GetFloatFieldInStruct(T* StructInstance, const FName& FieldName)
	{
		UStruct* StructType = StructInstance->StaticStruct();
		FProperty* Prop = StructType->FindPropertyByName(FieldName);
		const FFloatProperty* PropType = CastField<FFloatProperty>(Prop);
		float Ret = PropType->GetPropertyValue_InContainer(StructInstance);
		return Ret;
	}

	template <typename T>
	static int32 GetIntFieldInStruct(T* StructInstance, const FName& FieldName)
	{
		UStruct* StructType = StructInstance->StaticStruct();
		FProperty* Prop = StructType->FindPropertyByName(FieldName);
		const FIntProperty* PropType = CastField<FIntProperty>(Prop);
		int32 Ret = PropType->GetPropertyValue_InContainer(StructInstance);
		return Ret;
	}

	template <typename T>
	static bool GetBoolFieldInStruct(T* StructInstance, const FName& FieldName)
	{
		UStruct* StructType = StructInstance->StaticStruct();
		FProperty* Prop = StructType->FindPropertyByName(FieldName);
		const FBoolProperty* PropType = CastField<FBoolProperty>(Prop);
		bool Ret = PropType->GetPropertyValue_InContainer(StructInstance);
		return Ret;
	}

	static TArray<AActor*> GetFilteredCurrentActors(const TArray<AActor*>& InActors, const TArray<UClass*>& IgnoreClasses, const TArray<UClass*>& IncludeClasses);
	
	UFUNCTION(BlueprintCallable)
	static FString ReferencePathToFilePath(const FString& ReferencePath);

	UFUNCTION(BlueprintPure)
	static bool IsValueInRange(const int32 Value, const int32 A, const int32 B, bool GreaterEqual, bool LessEqual);
	
	UFUNCTION(BlueprintPure)
	static FVector GetCenterPointBetweenTwoVectors(FVector A, FVector B);
	
	UFUNCTION(BlueprintPure)
	static double FindAngleBetweenTwoVectors2D(FVector2D A, FVector2D B);

	UFUNCTION(BlueprintPure)
	static double FindAngleBetweenTwoVectors3D(FVector A, FVector B, EFindAngleAxis EFindAxis);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static bool SuggestProjectileVelocityAtLocation(const UObject* WorldContextObject, FVector& OutLaunchVelocity,
	                                                const FVector& ProjectileStartLocation, const FVector& TargetLocation, const float DesiredArcHeight,
	                                                double GravityZOverride, double TimeToTarget);

	UFUNCTION(BlueprintCallable)
	static FVector StringToVector3D( FString& InString);
	
	UFUNCTION(BlueprintCallable)
	static void DebugBreak(const FString& MessageString = TEXT("An intentional crash has occurred at the current node."));

	UFUNCTION(BlueprintPure)
	static TSoftClassPtr<UUserWidget> GetSoftWidgetClassFromPath(FString Path);	
	
	UFUNCTION(BlueprintPure)
	static bool IsEditorPlay();

	UFUNCTION(BlueprintCallable)
	static void EnableComponent(UActorComponent* Component, bool bEnable);

	UFUNCTION(BlueprintCallable)
	static FString GetLocalizedStringFromTable(const FName& TableRow);

	UFUNCTION(BlueprintCallable)
	static FVector GetBezierPointByDeCasteljau(const TArray<FVector>& Points, float t);


};