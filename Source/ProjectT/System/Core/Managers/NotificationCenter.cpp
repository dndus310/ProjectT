// Fill out your copyright notice in the Description page of Project Settings.


#include "NotificationCenter.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

UNotificationCenter::UNotificationCenter() :
	OuterObject(nullptr)
{
	
}

void UNotificationCenter::Initialize(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);
}

void UNotificationCenter::ReleaseNotificationCenter()
{
	for (TTuple<ENotificationTypes, TArray<FNotificationCenterItem>>& It : Observers)
	{
		for(FNotificationCenterItem& Item : It.Value)
		{
			if(Item.Subscribe.IsBound())
				Item.Subscribe.Unbind();

			if(Item.Object)
				Item.Object = nullptr;
		}
		It.Value.Empty();
	}
	Observers.Empty();
	OuterObject = nullptr;
}

void UNotificationCenter::AddObserver(const ENotificationTypes NotificationTypes,
	const FNotificationCenterItem& NotificationCenterItem)
{
	TArray<FNotificationCenterItem>& Items = Observers.FindOrAdd(NotificationTypes);
	Items.Emplace(NotificationCenterItem);
}

void UNotificationCenter::RemoveObserver(const UObject* Object, const ENotificationTypes NotificationTypes)
{
	if(NotificationTypes != ENotificationTypes::None)
	{
		const FString& NCName = UGlobalUtilsLibrary::GetEnumToString<ENotificationTypes>(NotificationTypes);
		if(!NMT_MSG_ENSURE(Observers.Contains(NotificationTypes), "%s Not Found", *NCName)) return;
			
		TArray<FNotificationCenterItem>& Items = Observers[NotificationTypes];
		Items.RemoveAll([&](FNotificationCenterItem& Item)
		{
			if(Item.Object == Object)
			{
				if(Item.Subscribe.IsBound()) Item.Subscribe.Unbind();
				Item.Object = nullptr;
				return true;
			}
			return false;
		});
	}
	else
	{
		for (TTuple<ENotificationTypes, TArray<FNotificationCenterItem>>& It : Observers)
		{
			It.Value.RemoveAll([&](FNotificationCenterItem& Item)
			{
				if (Item.Object == Object)
				{
					if(Item.Subscribe.IsBound()) Item.Subscribe.Unbind();
					Item.Object = nullptr;
					return true;
				}
				return false;
			});
		}	
	}
}

void UNotificationCenter::Post(const ENotificationTypes NotificationTypes)
{
	if(Observers.IsEmpty()) return;
	
	bool bPostFailed = NotificationTypes == ENotificationTypes::None || !Observers.Contains(NotificationTypes);
	if(bPostFailed) return;
		
	TArray<FNotificationCenterItem>& Items = Observers[NotificationTypes];
	for(FNotificationCenterItem Item : Items)
	{
		if(Item.Subscribe.IsBound())
			Item.Subscribe.Execute();
	}
}

void UNotificationCenter::AddObserverStatic(const UObject* Object, const ENotificationTypes NotificationTypes, const FNotificationCenterStatic& Subscribe)
{
	AddObserver(NotificationTypes, FNotificationCenterItem(Object, Subscribe));
}

void UNotificationCenter::AddObserverDynamic(const UObject* Object, const ENotificationTypes NotificationTypes, const FNotificationCenterDynamic& Subscribe)
{
	AddObserver(NotificationTypes, FNotificationCenterItem(Object, Subscribe));
}

void UNotificationCenter::RemoveObserverStatic(const UObject* Object, const ENotificationTypes NotificationTypes)
{
	RemoveObserver(Object, NotificationTypes);
}
