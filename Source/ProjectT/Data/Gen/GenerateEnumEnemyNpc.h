//Generated EnemyNpc Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EEnemyNpc : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Stationary UMETA(DisplayName= "Stationary"),
	Patrol UMETA(DisplayName= "Patrol"),
	Fixed UMETA(DisplayName= "Fixed"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEnemyNpc,EEnemyNpc::Stationary,EEnemyNpc::Fixed);

static FString EnumToString(EEnemyNpc InEnum)
{
	switch (InEnum)
	{
		case EEnemyNpc::Stationary: return TEXT("Stationary");
		case EEnemyNpc::Patrol: return TEXT("Patrol");
		case EEnemyNpc::Fixed: return TEXT("Fixed");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
