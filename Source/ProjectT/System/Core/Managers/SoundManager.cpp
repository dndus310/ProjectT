// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"

#include "Components/AudioComponent.h"
#include "ProjectT/ProjectT.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateStructSoundList.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"

USoundManager::USoundManager() :
	OuterObject(nullptr)
{
		
}

void USoundManager::InitializeManager(const UObject* WorldObjectContext)
{
	OuterObject = const_cast<UObject*>(WorldObjectContext);
}

UAudioComponent* USoundManager::PlaySound2D(FName RowName)
{
	return PlaySound2D(RowName, nullptr);
}

UAudioComponent* USoundManager::PlaySoundDynamic2D(FName RowName, const FOnSoundFinishedDynamic& Callback)
{
	UAudioComponent* Ret = PlaySound2D(RowName, FOnSoundFinishedStatic::CreateLambda([Callback](const ESoundState SoundState)
    {
    	if(Callback.IsBound()) Callback.Execute(SoundState);
    }));
	
	return Ret;
}

UAudioComponent* USoundManager::PlaySound2D(FName RowName, const FOnSoundFinishedStatic& Callback)
{
	FSoundDescription SoundDesc = GetSoundByContentTypeUnChecked(RowName);
	FString RowString = RowName.ToString();
	if(!CHECK_FREE(RowString))
	{
		if(!NMT_MSG_ENSURE(!SoundDesc.IsDefault(), "%s : Sound FilePath Does Not Exist!", *RowString)) return nullptr;
	}
	else return nullptr;
	
	UAudioComponent* Ret = UGameplayStatics::SpawnSound2D(this, SoundDesc.SoundBase);
	if(!Ret)
	{
		if(Callback.IsBound())
			Callback.Execute(ESoundState::NotSpawned);

		return nullptr;
	}

	FSoundElements SE(ESoundSpace::Space2D, RowName, Ret);
	SoundPool.Push(SE);

	if(Callback.IsBound())
	{
		OnSoundFinished = Callback;
		Ret->OnAudioFinished.AddDynamic(this, &USoundManager::OnSoundFinishedFunction);
	}
	UE_LOG(LogTemp, Log, TEXT("PlaySound[Space3D][%s] : %s"), *RowName.ToString(), *SoundDesc.SoundBase->GetPathName());

	return Ret;
}

void USoundManager::OnSoundFinishedFunction()
{
	if(OnSoundFinished.IsBound())
	{
		OnSoundFinished.Execute(ESoundState::Spawned);
		OnSoundFinished.Unbind();
	}
}

FSoundDescription USoundManager::GetSoundByContentTypeUnChecked(const FName& RowName)
{
	FString RowString = RowName.ToString();
	if(CHECK_FREE(RowString) || !RowString.IsNumeric()) return FSoundDescription();

	FSoundDescription Ret;
	UGameInstance* GI = UGameplayStatics::GetGameInstance(OuterObject);
	NMT_CHECKF(GI);
	
	UDataDam* DD = IGlobalObjectProvider::Execute_GetDataDam(GI);
	NMT_CHECKF(DD);

	const UDataTable* FoundTable = DD->GetDataTableByEnum(EDataTableKey::SoundList);
	uint8* FoundData = FoundTable->FindRowUnchecked(RowName);
	if(!NMT_MSG_ENSURE(FoundData, "Not Found Sound Data : %s", *RowName.ToString()))
		return FSoundDescription();

	FSoundList* SoundData = reinterpret_cast<FSoundList*>(FoundData);
	if(SoundData->sSoundClass == TEXT("BGM"))
	{
		Ret.SoundBase = UAssetUtilsLibrary::GetSoundBaseFromPath(SoundData->sSoundPath);
		Ret.Space = ESoundSpace::Space2D;
	}
	else if(SoundData->sSoundClass == TEXT("SFX"))
	{
		Ret.SoundBase = UAssetUtilsLibrary::GetSoundBaseFromPath(SoundData->sSoundPath);
		Ret.Space = SoundData->sSoundType.Equals(TEXT("2D"))? ESoundSpace::Space2D : ESoundSpace::Space3D;

		if(!CHECK_FREE(SoundData->sAttenPath))
			Ret.SoundAttenuation = UAssetUtilsLibrary::GetSoundAttenuationFromPath(SoundData->sAttenPath);
	}
	else NMT_CHECKF(0);
	
	return Ret;
}

UAudioComponent* USoundManager::PlaySound3D(FName RowName, FVector Location)
{
	return PlaySound3D(RowName, Location, nullptr);
}

UAudioComponent* USoundManager::PlaySoundDynamic3D(FName RowName, FVector Location, const FOnSoundFinishedDynamic& Callback)
{
	UAudioComponent* Ret = PlaySound3D(RowName, Location, FOnSoundFinishedStatic::CreateLambda([Callback](const ESoundState SoundState)
	{
		if(Callback.IsBound()) Callback.Execute(SoundState);
	}));
	
	return Ret;
}

UAudioComponent* USoundManager::PlaySound3D(FName RowName, FVector Location, const FOnSoundFinishedStatic& CallBack)
{
	UAudioComponent* Ret = nullptr;
	FSoundDescription SoundDesc = GetSoundByContentTypeUnChecked(RowName);
	FString RowString = RowName.ToString();

	if(!CHECK_FREE(RowString))
	{
		if(!NMT_MSG_ENSURE(!SoundDesc.IsDefault(), "%s : Sound FilePath Does Not Exist!", *RowString)) return nullptr;
	}
	else return nullptr;
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(OuterObject, 0);
	Ret = UGameplayStatics::SpawnSoundAtLocation(OuterObject, SoundDesc.SoundBase, Location, FRotator(), 1.f, 1.f, 0.f, SoundDesc.SoundAttenuation);
	if(!Ret)
	{
		if(CallBack.IsBound())
			CallBack.Execute(ESoundState::NotSpawned);

		return nullptr;
	}
	
	FSoundElements SE(ESoundSpace::Space3D, RowName, Ret);
	SoundPool.Push(SE);

	if(CallBack.IsBound())
	{
		OnSoundFinished = CallBack;
		Ret->OnAudioFinished.AddDynamic(this, &USoundManager::OnSoundFinishedFunction);
	}
	UE_LOG(LogTemp, Log, TEXT("PlaySound[Space3D][%s] : %s"), *RowName.ToString(), *SoundDesc.SoundBase->GetPathName());
	
	return Ret;
}

UAudioComponent* USoundManager::PlaySound3DAttached(FName RowName, USceneComponent* AttachComponent,
                                        const EAttachLocation::Type AttachLocationRule, FString SocketID)
{
	UAudioComponent* Ret = nullptr;
	FSoundDescription SoundDesc = GetSoundByContentTypeUnChecked(RowName);
	FString RowString = RowName.ToString();
	
	if(!CHECK_FREE(RowString))
	{
		if(!NMT_MSG_ENSURE(!SoundDesc.IsDefault(), "%s : Sound FilePath Does Not Exist!", *RowString)) return nullptr;
		if(!NMT_MSG_ENSURE(AttachComponent, "%s : AttachComponent Is Null!", *RowString)) return nullptr;
	}
	else return nullptr;

	APlayerController* PC = UGameplayStatics::GetPlayerController(OuterObject, 0);
	Ret = UGameplayStatics::SpawnSoundAttached(SoundDesc.SoundBase, AttachComponent, *SocketID, FVector(), FRotator(), AttachLocationRule, false, 1.f, 1.f, 0.f, SoundDesc.SoundAttenuation);
	if(!Ret)
	{
		return nullptr;
	}
		
	
	FSoundElements SE(ESoundSpace::Space3D, RowName, Ret);
	SoundPool.Push(SE);

	UE_LOG(LogTemp, Log, TEXT("PlaySound[Space3D][%s] : %s"), *RowName.ToString(), *SoundDesc.SoundBase->GetPathName());
	
	return Ret;
}

void USoundManager::ResetAllPoolingSounds()
{
	for(const FSoundElements& SE : SoundPool)
	{
		if(IsValid(SE.AudioComponent) && IsValid(SE.AudioComponent->GetSound()))
		{
			if(SE.AudioComponent->IsPlaying())
				SE.AudioComponent->Stop();
		}
	}
	SoundPool.Empty();
}

void USoundManager::ReleaseManager()
{
	for(const FSoundElements& SE : SoundPool)
	{
		if(IsValid(SE.AudioComponent))
			SE.AudioComponent->DestroyComponent();
	}
	SoundPool.Empty();
}

