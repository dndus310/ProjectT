// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SoundManager.generated.h"

namespace EAttachLocation
{
	enum Type : int;	
};

UENUM()
enum class ESoundSpace : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Space2D UMETA(DisplayName = "Space2D"),
	Space3D UMETA(DisplayName = "Space3D"),
};

UENUM(BlueprintType)
enum class ESoundState : uint8
{
	NotSpawned = 0 UMETA(DisplayName = "NotSpawned"),
	Spawned UMETA(DisplayName = "Spawned"),
};

USTRUCT(Blueprintable)
struct FSoundElements
{
	GENERATED_BODY()

	FSoundElements(): Space(ESoundSpace::None), AudioComponent(nullptr) {}
	FSoundElements(ESoundSpace InSpace, const FName& InSoundName, UAudioComponent* InAudioComponent)
			: Space(InSpace), SoundName(InSoundName), AudioComponent(InAudioComponent) {}

	UPROPERTY()
	ESoundSpace Space;

	UPROPERTY()
	FName SoundName;

	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent;
};


USTRUCT(Blueprintable)
struct FSoundDescription
{
	GENERATED_BODY()

	FSoundDescription() : SoundBase(nullptr), SoundAttenuation(nullptr), Space(ESoundSpace::None){}
	FSoundDescription(USoundBase* const InSoundBase, USoundAttenuation* const InSoundAttenuation, const ESoundSpace InSpace) :
		SoundBase(InSoundBase),
		SoundAttenuation(InSoundAttenuation),
		Space(InSpace){}

	bool IsDefault() const
	{
		return !SoundBase && !SoundAttenuation;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> SoundBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundAttenuation> SoundAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESoundSpace Space;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSoundFinishedDynamic, const ESoundState, SoundState);
DECLARE_DELEGATE_OneParam(FOnSoundFinishedStatic, const ESoundState);
UCLASS()
class PROJECTT_API USoundManager : public UObject
{
	GENERATED_BODY()
	
public:
	USoundManager();

	void InitializeManager(const UObject* WorldObjectContext);
	void ReleaseManager();
	
	UFUNCTION()
	void OnSoundFinishedFunction();

	UAudioComponent* PlaySound2D(FName RowName, const FOnSoundFinishedStatic& Callback);
	
	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySound2D(FName RowName);

	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySoundDynamic2D(FName RowName, const FOnSoundFinishedDynamic& Callback);

	UAudioComponent* PlaySound3D(FName RowName, FVector Location, const FOnSoundFinishedStatic& CallBack);
	
	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySound3D(FName RowName, FVector Location);

	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySoundDynamic3D(FName RowName, FVector Location, const FOnSoundFinishedDynamic& Callback);

	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySound3DAttached(FName RowName, USceneComponent* AttachComponent, const EAttachLocation::Type AttachLocationRule, FString SocketID);

	UFUNCTION(BlueprintCallable)
	void ResetAllPoolingSounds();
	
private:
	FSoundDescription GetSoundByContentTypeUnChecked(const FName& RowName);

private:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;

	UPROPERTY()
	TArray<FSoundElements> SoundPool;
	
	FOnSoundFinishedStatic OnSoundFinished;
};
