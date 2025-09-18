//Generated ContentType Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EContentType : uint8 
{
	None = 0 UMETA(DisplayName = "None"),
	Stage = 10 UMETA(DisplayName= "Stage"),
	Objective = 11 UMETA(DisplayName= "Objective"),
	CharacterUnlock = 14 UMETA(DisplayName= "CharacterUnlock"),
	PlayerCharacter = 20 UMETA(DisplayName= "PlayerCharacter"),
	EnemyNpc = 21 UMETA(DisplayName= "EnemyNpc"),
	ObjectiveNpc = 22 UMETA(DisplayName= "ObjectiveNpc"),
	Effect = 30 UMETA(DisplayName= "Effect"),
	Skill = 31 UMETA(DisplayName= "Skill"),
	Item = 40 UMETA(DisplayName= "Item"),
	ItemSet = 41 UMETA(DisplayName= "ItemSet"),
	LevelObject = 42 UMETA(DisplayName= "LevelObject"),
	Localization = 60 UMETA(DisplayName= "Localization"),
	Mesh = 80 UMETA(DisplayName= "Mesh"),
	MeshSet = 81 UMETA(DisplayName= "MeshSet"),
	Animation = 82 UMETA(DisplayName= "Animation"),
	Sound = 91 UMETA(DisplayName= "Sound"),
	Haptic = 93 UMETA(DisplayName= "Haptic"),
	Vfx = 94 UMETA(DisplayName= "Vfx"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EContentType,EContentType::Stage,EContentType::Vfx);

static FString EnumToString(EContentType InEnum)
{
	switch (InEnum)
	{
		case EContentType::Stage: return TEXT("Stage");
		case EContentType::Objective: return TEXT("Objective");
		case EContentType::CharacterUnlock: return TEXT("CharacterUnlock");
		case EContentType::PlayerCharacter: return TEXT("PlayerCharacter");
		case EContentType::EnemyNpc: return TEXT("EnemyNpc");
		case EContentType::ObjectiveNpc: return TEXT("ObjectiveNpc");
		case EContentType::Effect: return TEXT("Effect");
		case EContentType::Skill: return TEXT("Skill");
		case EContentType::Item: return TEXT("Item");
		case EContentType::ItemSet: return TEXT("ItemSet");
		case EContentType::LevelObject: return TEXT("LevelObject");
		case EContentType::Localization: return TEXT("Localization");
		case EContentType::Mesh: return TEXT("Mesh");
		case EContentType::MeshSet: return TEXT("MeshSet");
		case EContentType::Animation: return TEXT("Animation");
		case EContentType::Sound: return TEXT("Sound");
		case EContentType::Haptic: return TEXT("Haptic");
		case EContentType::Vfx: return TEXT("Vfx");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
