//Generated DataTableKey Index to Enum

#pragma once

UENUM(BlueprintType)
enum class EDataTableKey : uint8 
{
	AnimationSet UMETA(DisplayName = "AnimationSet"),
	CharacterMeshSet UMETA(DisplayName = "CharacterMeshSet"),
	CharacterUnlock UMETA(DisplayName = "CharacterUnlock"),
	Effect UMETA(DisplayName = "Effect"),
	EnemyNpc UMETA(DisplayName = "EnemyNpc"),
	HapticList UMETA(DisplayName = "HapticList"),
	Item UMETA(DisplayName = "Item"),
	ItemSet UMETA(DisplayName = "ItemSet"),
	LevelObject UMETA(DisplayName = "LevelObject"),
	Localization UMETA(DisplayName = "Localization"),
	MeshList UMETA(DisplayName = "MeshList"),
	Objective UMETA(DisplayName = "Objective"),
	PlayerCharacter UMETA(DisplayName = "PlayerCharacter"),
	Skill UMETA(DisplayName = "Skill"),
	SocketList UMETA(DisplayName = "SocketList"),
	SoundList UMETA(DisplayName = "SoundList"),
	Stage UMETA(DisplayName = "Stage"),
	VfxList UMETA(DisplayName = "VfxList"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDataTableKey,EDataTableKey::AnimationSet,EDataTableKey::VfxList);

static FString EnumToString(EDataTableKey InEnum)
{
	switch (InEnum)
	{
		case EDataTableKey::AnimationSet: return TEXT("AnimationSet");
		case EDataTableKey::CharacterMeshSet: return TEXT("CharacterMeshSet");
		case EDataTableKey::CharacterUnlock: return TEXT("CharacterUnlock");
		case EDataTableKey::Effect: return TEXT("Effect");
		case EDataTableKey::EnemyNpc: return TEXT("EnemyNpc");
		case EDataTableKey::HapticList: return TEXT("HapticList");
		case EDataTableKey::Item: return TEXT("Item");
		case EDataTableKey::ItemSet: return TEXT("ItemSet");
		case EDataTableKey::LevelObject: return TEXT("LevelObject");
		case EDataTableKey::Localization: return TEXT("Localization");
		case EDataTableKey::MeshList: return TEXT("MeshList");
		case EDataTableKey::Objective: return TEXT("Objective");
		case EDataTableKey::PlayerCharacter: return TEXT("PlayerCharacter");
		case EDataTableKey::Skill: return TEXT("Skill");
		case EDataTableKey::SocketList: return TEXT("SocketList");
		case EDataTableKey::SoundList: return TEXT("SoundList");
		case EDataTableKey::Stage: return TEXT("Stage");
		case EDataTableKey::VfxList: return TEXT("VfxList");
		default: checkf(false, TEXT("Error"));
	}
	return FString();
}
