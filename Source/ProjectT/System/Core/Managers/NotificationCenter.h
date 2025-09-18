// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectT/ProjectT.h"
#include "UObject/Object.h"
#include "NotificationCenter.generated.h"

UENUM(BlueprintType)
enum class ENotificationTypes : uint8
{
	None = 0							UMETA(DisplayName = "None"),
	Option_Language_Changed				UMETA(DisplayName = "Option_Language_Changed"),
	Level_Transition					UMETA(DisplayName = "Level_Transition"),
	GameState_Started					UMETA(DisplayName = "GameState_Started"),
	GameState_Changed					UMETA(DisplayName = "GameState_Changed"),
	GameState_Restarted					UMETA(DisplayName = "GameState_Restarted"),
	GameState_Defeat					UMETA(DisplayName = "GameState_Defeat"),
	GameState_Victory					UMETA(DisplayName = "GameState_Victory"),
	Player_Dead							UMETA(DisplayName = "Player_Dead"),
	UI_DefeatActionCleared				UMETA(DisplayName = "UI_DefeatActionCleared"),
	UI_VictoryActionCleared				UMETA(DisplayName = "UI_VictoryActionCleared"),
};

DECLARE_DELEGATE(FNotificationCenterStatic)
DECLARE_DYNAMIC_DELEGATE(FNotificationCenterDynamic);

struct FIntegratedSubscribeDelegate
{
	FNotificationCenterStatic FunctionStatic;
	FNotificationCenterDynamic FunctionDynamic;

	FIntegratedSubscribeDelegate(){}
	FIntegratedSubscribeDelegate(FNotificationCenterStatic const& Delegate) : FunctionStatic(Delegate){}
	FIntegratedSubscribeDelegate(FNotificationCenterDynamic const& Delegate) : FunctionDynamic(Delegate){}

	void Execute()
	{
		if (FunctionStatic.IsBound()) FunctionStatic.Execute();
		else if (FunctionDynamic.IsBound()) FunctionDynamic.Execute();
		else NMT_MSG_CHECKF(0, "FIntegratedSubscribeDelegate Does Not Bound!");
	}
	bool IsBound() const
	{
		return FunctionStatic.IsBound() || FunctionDynamic.IsBound();
	}
	void Unbind()
	{
		FunctionStatic.Unbind();
		FunctionDynamic.Unbind();
	}
};

USTRUCT(BlueprintType)
struct FNotificationCenterItem
{
	GENERATED_BODY()

	FNotificationCenterItem() : Object(nullptr), Subscribe(nullptr){}
	FNotificationCenterItem(const UObject* Object, FIntegratedSubscribeDelegate const& Subscribe) : Object(Object), Subscribe(Subscribe){}

	bool operator==(const FNotificationCenterItem& Other) const
	{
		return Object == Other.Object;
	}

	bool operator!=(const FNotificationCenterItem& Other) const
	{
		return !(*this == Other);
	}

	UPROPERTY()
	const UObject* Object;

	FIntegratedSubscribeDelegate Subscribe;
};

enum class ENotificationTypes : uint8;

UCLASS()
class PROJECTT_API UNotificationCenter : public UObject
{
	GENERATED_BODY()
	
	using TObservers = TMap<ENotificationTypes, TArray<FNotificationCenterItem>>;

public:
	UNotificationCenter();
	
	virtual void AddObserverStatic(const UObject* Object, const ENotificationTypes NotificationTypes, const FNotificationCenterStatic& Subscribe);
	virtual void RemoveObserverStatic(const UObject* Object, const ENotificationTypes NotificationTypes);

	void Initialize(const UObject* WorldObjectContext);
	void ReleaseNotificationCenter();
	
	UFUNCTION(BlueprintCallable)
	virtual void AddObserverDynamic(const UObject* Object, const ENotificationTypes NotificationTypes, const FNotificationCenterDynamic& Subscribe);
	
	UFUNCTION(BlueprintCallable)
	virtual void RemoveObserver(const UObject* Object, const ENotificationTypes NotificationTypes);
	
	UFUNCTION(BlueprintCallable)
	virtual void Post(const ENotificationTypes NotificationTypes);

private:
	void AddObserver(const ENotificationTypes NotificationTypes, const FNotificationCenterItem& NotificationCenterItem);
	
private:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;

	TObservers Observers;
};
