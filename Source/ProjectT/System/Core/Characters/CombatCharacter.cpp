#include "CombatCharacter.h"
#include "Components/CapsuleComponent.h"
#include "ProjectT/ProjectT.h"
#include "ProjectT/Data/Gen/GenerateEnumContentType.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateStructCharacterMeshSet.h"
#include "ProjectT/Data/Gen/GenerateStructEnemyNPC.h"
#include "ProjectT/Data/Gen/GenerateStructMeshList.h"
#include "ProjectT/Data/Gen/GenerateStructPlayerCharacter.h"
#include "ProjectT/Data/Gen/GenerateStructSocketList.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"

class CharacterDataLoader
{
public:
	static void From(const FName& RowName, FName& OutMeshRowName, FCharacterMeshSet& OutMeshList, const FTableRowBase*& OutContentData)
	{
		FName FoundMeshRowName;
		EContentType NewContentType = UGlobalUtilsLibrary::GetContentTypeFromIndex(RowName);
		UDataTable* DataTable = nullptr;
		switch(NewContentType)
		{
			case EContentType::PlayerCharacter:
			{
				DataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::PlayerCharacter);
				FPlayerCharacter* CharacterDataRow = DataTable->FindRow<FPlayerCharacter>(RowName, TEXT("Not Found FPlayerCharacter"));
				if(!NMT_ENSURE(CharacterDataRow)) return;
				FoundMeshRowName = FROM_INT(CharacterDataRow->iCharMeshSetId);
				OutContentData = CharacterDataRow;
				break;
			}
			case EContentType::EnemyNpc:
			{
				DataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::EnemyNpc);
				FEnemyNpc* CharacterDataRow = DataTable->FindRow<FEnemyNpc>(RowName, TEXT("Not Found FEnemyNPC"));
				if(!NMT_ENSURE(CharacterDataRow)) return;
				FoundMeshRowName = FROM_INT(CharacterDataRow->iNpcMeshSetId);
				OutContentData = CharacterDataRow;
				break;
			}
			default: return;
		}
		DataTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::CharacterMeshSet);
		if(FCharacterMeshSet* RowData = DataTable->FindRow<FCharacterMeshSet>(FoundMeshRowName, TEXT("FCharacterMeshSet")))
		{
			OutMeshList = *RowData;
			OutMeshRowName = FoundMeshRowName;
		}
	}
};

ACombatCharacter::ACombatCharacter() :
	AccessoryMeshComponent(nullptr),
	WeaponStaticComponent(nullptr),
	WeaponDynamicComponent(nullptr),
	DefaultBodyMesh(nullptr)
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
	GetCapsuleComponent()->SetCapsuleRadius(20.0f);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -52.0f));

	AccessoryMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AccessoryMeshComponent"));
	WeaponStaticComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticComponent"));
	WeaponDynamicComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponDynamicComponent"));
	NMT_CHECKF(AccessoryMeshComponent && WeaponStaticComponent && WeaponDynamicComponent);

	UGlobalUtilsLibrary::EnableComponent(AccessoryMeshComponent, false);
	UGlobalUtilsLibrary::EnableComponent(WeaponStaticComponent, false);
	UGlobalUtilsLibrary::EnableComponent(WeaponDynamicComponent, false);
}

void ACombatCharacter::PostLoad()
{
	Super::PostLoad();
}

void ACombatCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	const FString LabelRowName = *GetActorLabel();
	if(LabelRowName.IsNumeric()) ApplyCharacterConfiguration(*LabelRowName);
#endif
}

EContentType ACombatCharacter::GetContentType_Implementation()
{
	return IContentType::GetContentType_Implementation();
}

FString ACombatCharacter::GetContentTypeNumber_Implementation()
{
	return IContentType::GetContentTypeNumber_Implementation();
}

void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	const FString LabelRowName = *GetActorLabel();
	if(LabelRowName.IsNumeric()) ApplyCharacterConfiguration(*LabelRowName);
	else if(!CharacterRowName.IsNone() && CharacterRowName.ToString().IsNumeric()) ApplyCharacterConfiguration(CharacterRowName);
#endif

	GetCapsuleComponent()->OnClicked.AddUniqueDynamic(this, &ACombatCharacter::ExecuteClickedEvent);
	GetCapsuleComponent()->OnBeginCursorOver.AddUniqueDynamic(this, &ACombatCharacter::ExecuteBeginCursorOverEvent);
	GetCapsuleComponent()->OnEndCursorOver.AddUniqueDynamic(this, &ACombatCharacter::ExecuteEndCursorOverEvent);
}

#if WITH_EDITOR
void ACombatCharacter::ApplyCharacterConfiguration(const FName& RowName)
{
	if(RowName.IsNone())
	{
		RestoreCharacterSet();
		RestoreCharacterData();
		return;
	}

	const EContentType NewContentType = UGlobalUtilsLibrary::GetContentTypeFromIndex(RowName);
	if(EContentType::PlayerCharacter != NewContentType && EContentType::EnemyNpc != NewContentType)
	{
		RestoreCharacterSet();
		RestoreCharacterData();
		return;
	}

	FName MeshSetRowName;
	FCharacterMeshSet NewMeshSet;
	const FTableRowBase* CharacterData = nullptr;
	CharacterDataLoader::From(RowName, MeshSetRowName, NewMeshSet, CharacterData);
	if(MeshSetRowName.IsNone())
	{
		RestoreCharacterSet();
		return;
	}

	if(ApplyCharacterSet(MeshSetRowName))
	{
		CurrentMeshSet = NewMeshSet;
		CharacterRowName = RowName;
	}
	else RestoreCharacterSet();

	if(CharacterData) ApplyCharacterData(CharacterData);
}

bool ACombatCharacter::ApplyCharacterSet(const FName& RowName)
{
	UDataTable* CMSTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::CharacterMeshSet);
	FCharacterMeshSet* CMSData = CMSTable->FindRow<FCharacterMeshSet>(RowName, TEXT("FCharacterMeshSet"));
	if(!CMSData) return false;

	UDataTable* MLSTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::MeshList);
	FMeshList* BodyData = MLSTable->FindRow<FMeshList>(FROM_INT(CMSData->iBodyMeshId), TEXT("FMeshList"));
	if(!BodyData) return false;

	USkeletalMesh* BodyMesh = UAssetUtilsLibrary::GetSkeletalMeshFromPath(BodyData->sMeshPath);
	if(!BodyMesh) return false;
	GetMesh()->SetSkeletalMesh(BodyMesh);

	FMeshList* WeaponData = nullptr;
	if(CMSData->iWeaponMeshId != 0) WeaponData = MLSTable->FindRow<FMeshList>(FROM_INT(CMSData->iWeaponMeshId), TEXT("FMeshList"));

	/**
	 * NOTE: Weapon Data는 항상 유효성을 보장하지 않음
	 */
	if(!WeaponData) return true;
	if(!CHECK_FREE(WeaponData->sMeshPath))
	{
		UMeshComponent* WeaponComponent = nullptr;
		FString ScriptType = UGlobalUtilsLibrary::GetScriptTypeByRefPath(WeaponData->sMeshPath);
		if(ScriptType.Equals(TEXT("StaticMesh")))
		{
			UStaticMesh* WeaponMesh = UAssetUtilsLibrary::GetStaticMeshFromPath(WeaponData->sMeshPath);
			WeaponStaticComponent->SetStaticMesh(WeaponMesh);
			UGlobalUtilsLibrary::EnableComponent(WeaponStaticComponent, true);
			WeaponComponent = WeaponStaticComponent;
		}
		else if(ScriptType.Equals(TEXT("SkeletalMesh")))
		{
			USkeletalMesh* WeaponMesh = UAssetUtilsLibrary::GetSkeletalMeshFromPath(WeaponData->sMeshPath);
			WeaponDynamicComponent->SetSkeletalMesh(WeaponMesh);
			UGlobalUtilsLibrary::EnableComponent(WeaponDynamicComponent, true);
			WeaponComponent = WeaponDynamicComponent;
		}
		else NMT_MSG_CHECKF(0, "%s: type is not supported", *WeaponData->sMeshPath);

		CMSData->iWeaponSocketId;
		UDataTable* SocketTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::SocketList);
		FSocketList* SocketData = SocketTable->FindRow<FSocketList>(FROM_INT(CMSData->iWeaponSocketId), TEXT("FSocketList"));
		if(CHECK_FREE(SocketData->sSocketName))
		{
			NMT_MSG_ENSURE(0, "Socket Name Is FREE [%s][%s]", FROM_INT(CMSData->iWeaponSocketId), *SocketData->sSocketName);
			return true;
		}

		WeaponComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, *SocketData->sSocketName);
	}

	return true;
}

void ACombatCharacter::ApplyCharacterData(const FTableRowBase* RowData)
{
	NMT_MSG_ENSURE(0, "Function should be overridden in the child class.");
}

void ACombatCharacter::RestoreCharacterData()
{
	ApplyCharacterData(nullptr);
}

void ACombatCharacter::RestoreCharacterSet()
{
	AccessoryMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	UGlobalUtilsLibrary::EnableComponent(AccessoryMeshComponent, false);

	WeaponStaticComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	UGlobalUtilsLibrary::EnableComponent(WeaponStaticComponent, false);

	WeaponDynamicComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	UGlobalUtilsLibrary::EnableComponent(WeaponDynamicComponent, false);

	FString LocDefaultSKMeshPath = "/Script/Engine.SkeletalMesh'/Game/Characters/Pyro/Assets/SK_Pyro_Invalid.SK_Pyro_Invalid'";
	DefaultBodyMesh = UAssetUtilsLibrary::GetSkeletalMeshFromPath(LocDefaultSKMeshPath);
	if(DefaultBodyMesh != nullptr)
	{
		GetMesh()->SetSkeletalMesh(DefaultBodyMesh);
	}

	CurrentMeshSet = FCharacterMeshSet();
	CharacterRowName = TEXT("None");
}
#endif

void ACombatCharacter::Respawn()
{
	NOT_IMPLEMENTED();
}

void ACombatCharacter::ExecuteClickedEvent(UPrimitiveComponent* InTouchedComponent, FKey InButtonPressed)
{
	Super::ExecuteClickedEvent(InTouchedComponent, InButtonPressed);
}

void ACombatCharacter::ExecuteBeginCursorOverEvent(UPrimitiveComponent* InTouchedComponent)
{
	Super::ExecuteBeginCursorOverEvent(InTouchedComponent);
}

void ACombatCharacter::ExecuteEndCursorOverEvent(UPrimitiveComponent* InTouchedComponent)
{
	Super::ExecuteEndCursorOverEvent(InTouchedComponent);
}
