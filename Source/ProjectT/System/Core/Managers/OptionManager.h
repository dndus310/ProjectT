// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionManager.generated.h"

// UENUM(BlueprintType)
// enum class EGameLanguage : uint8
// {
// 	Korea = 0 UMETA(DisplayName = "Korea"),
// 	English UMETA(DisplayName = "English"),
// 	Japanese UMETA(DisplayName = "Japanese"),
// 	ChineseCN UMETA(DisplayName = "ChineseCN"),
// 	ChineseTW UMETA(DisplayName = "ChineseTW"),
// 	Spanish UMETA(DisplayName = "Spanish"),
// 	French UMETA(DisplayName = "French"),
// 	German UMETA(DisplayName = "German"),
// 	Portugese UMETA(DisplayName = "Portugese"),
// 	Russian UMETA(DisplayName = "Russian"),
// };
//
//
// USTRUCT(Blueprintable)
// struct FGamePlayOption
// {
// 	GENERATED_BODY()
//
// 	FGamePlayOption() :
// 		HardwareLanguage(EGameLanguage::English),
// 		Language(EGameLanguage::English)
// 	{
// 	}
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlayOption")
// 	EGameLanguage HardwareLanguage;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlayOption")
// 	EGameLanguage Language;
// };

// USTRUCT(Blueprintable)
// struct FAudioOption
// {
// 	GENERATED_BODY()
//
// 	FAudioOption() :
// 		BGMVolume(5),
// 		SFXVolume(5)
// 	{
// 	}
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AudioOption")
// 	int BGMVolume;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AudioOption")
// 	int SFXVolume;
// };

//USTRUCT(Blueprintable)
// struct FGameOptionData
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OptionData")
// 	FGamePlayOption GamePlayOption;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OptionData")
// 	FAudioOption AudioOption;
// };

UCLASS()
class PROJECTT_API UOptionManager : public UObject
{
	GENERATED_BODY()

public:
	UOptionManager();
	
	void InitializeManager(const UObject* WorldObjectContext);

	// void InitializeOptionData();

// 	UFUNCTION(BlueprintPure)
// 	FGamePlayOption GetGamePlayOption() const;
//
// 	UFUNCTION(BlueprintPure)
// 	FAudioOption GetAudioOption() const;
//
// 	UFUNCTION(BlueprintCallable)
// 	void SetGamePlayOption(FGamePlayOption OptionData);
//
// 	UFUNCTION(BlueprintCallable)
// 	void SetAudioOption(FAudioOption OptionData);
//
// 	UFUNCTION(BlueprintCallable)
// 	void ResetGameOptionData();
//
 	void ReleaseManager();
	
private:
	UPROPERTY()
	TObjectPtr<UObject> OuterObject;
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameOptionData", meta = (AllowPrivateAccess = "true"))
// 	FGameOptionData GameOptionData;

};
