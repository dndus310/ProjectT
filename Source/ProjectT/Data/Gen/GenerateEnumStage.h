//Generated Stage Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EStage : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	L_SP_Office_01 UMETA(DisplayName= "L_SP_Office_01"),
	L_SP_Office_02 UMETA(DisplayName= "L_SP_Office_02"),
	L_SP_Office_03 UMETA(DisplayName= "L_SP_Office_03"),
	L_SP_Office_04 UMETA(DisplayName= "L_SP_Office_04"),
	L_SP_Office_05 UMETA(DisplayName= "L_SP_Office_05"),
	L_SP_Test_00 UMETA(DisplayName= "L_SP_Test_00"),
	L_Entry UMETA(DisplayName= "L_Entry"),
	L_Lobby UMETA(DisplayName= "L_Lobby"),
	L_Intro UMETA(DisplayName= "L_Intro"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EStage,EStage::L_SP_Office_01,EStage::L_Intro);

static FString EnumToString(EStage InEnum)
{
	switch (InEnum)
	{
		case EStage::L_SP_Office_01: return TEXT("L_SP_Office_01");
		case EStage::L_SP_Office_02: return TEXT("L_SP_Office_02");
		case EStage::L_SP_Office_03: return TEXT("L_SP_Office_03");
		case EStage::L_SP_Office_04: return TEXT("L_SP_Office_04");
		case EStage::L_SP_Office_05: return TEXT("L_SP_Office_05");
		case EStage::L_SP_Test_00: return TEXT("L_SP_Test_00");
		case EStage::L_Entry: return TEXT("L_Entry");
		case EStage::L_Lobby: return TEXT("L_Lobby");
		case EStage::L_Intro: return TEXT("L_Intro");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
