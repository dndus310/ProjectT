// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelCastObject.h"

void ALevelCastObject::TryInteraction(const FInteractionData InSendData, FOnInteractionSuccess Callback)
{
	if (LevelObjectData.iCastItemId != 0)
	{
		if(InSendData.RowNames.Num()>0)
		{
			if(InSendData.RowNames.Contains(FString::FromInt(LevelObjectData.iCastItemId)))
			{
				if(Callback.IsBound()) Callback.Execute(EInteractionResultType::SuccessPre);
				SuccessInteraction(true);		
				ExecuteBindNPCEvent();
				ExecuteBindCeilEvent();
			}
			else
			{
				if(Callback.IsBound()) Callback.Execute(EInteractionResultType::Fail);
				SuccessInteraction(false);				
			}
		}
		else
		{
			if(Callback.IsBound()) Callback.Execute(EInteractionResultType::Fail);
			SuccessInteraction(false);
		}
	}
	else
	{
		if(Callback.IsBound()) Callback.Execute(EInteractionResultType::SuccessPre);
		SuccessInteraction(true);
		ExecuteBindNPCEvent();
		ExecuteBindCeilEvent();
	}
}

void ALevelCastObject::SuccessPreAction_Implementation()
{
	Super::SuccessPreAction_Implementation();
}

void ALevelCastObject::SuccessUpdateAction_Implementation()
{
	Super::SuccessUpdateAction_Implementation();
}

void ALevelCastObject::SuccessCompleteAction_Implementation()
{
	Super::SuccessCompleteAction_Implementation();
}

void ALevelCastObject::SetVisibility(bool InbVisible)
{
	Super::SetVisibility(InbVisible);
}

void ALevelCastObject::ShowVisual_Implementation()
{
	Super::ShowVisual_Implementation();

	TArray<USceneComponent*> ChildComponents;
	for (auto Child : ChildComponents)
	{
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Child))
		{			
			PrimitiveComp->SetCustomDepthStencilValue(0);
		}
	}
}

void ALevelCastObject::HideVisual_Implementation()
{
	Super::HideVisual_Implementation();

	TArray<USceneComponent*> ChildComponents;
	for (auto Child : ChildComponents)
	{
		if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Child))
		{
			PrimitiveComp->SetCustomDepthStencilValue(1);			
		}
	}
}

void ALevelCastObject::SuccessInteraction_Implementation(bool Result)
{
	
}
