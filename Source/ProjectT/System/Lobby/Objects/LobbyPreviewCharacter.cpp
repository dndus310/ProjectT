
#include "LobbyPreviewCharacter.h"
#include "ProjectT/System/Core/Interfaces/GlobalObjectProvider.h"
#include "ProjectT/System/Core/Managers/DataDam.h"
#include "ProjectT/Data/Gen/GenerateEnumDataTableKey.h"
#include "ProjectT/Data/Gen/GenerateStructMeshList.h"
#include "ProjectT/Data/Gen/GenerateStructSocketList.h"
#include "ProjectT/Data/Gen/GenerateStructCharacterMeshSet.h"
#include "ProjectT/Data/Gen/GenerateStructAnimationSet.h"
#include "ProjectT/System/Core/Common/AssetUtilsLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/SaveGame/UserSaveGame.h"


// Sets default values
ALobbyPreviewCharacter::ALobbyPreviewCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterBody"));
	BodyMesh->SetupAttachment(RootComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetupAttachment(BodyMesh);

	WeaponDynamicMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponDynamic"));
	WeaponDynamicMesh->SetupAttachment(BodyMesh);
}

// Called when the game starts or when spawned
void ALobbyPreviewCharacter::BeginPlay()
{
	Super::BeginPlay();

	UUserSaveGame* SG; 
	UUserSaveGame::LoadUserSaveGame(SG);
	if(!SG) return;

	FString SelectData;
	SG->GetSelectedCharacterData(SelectData);

	UDataTable* ChrTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::PlayerCharacter);
	FPlayerCharacter* ChrData = ChrTable->FindRow<FPlayerCharacter>(*SelectData, TEXT("FPlayerCharacter"));
	if(!ChrData) return;

	SetCharacterByData(*ChrData);	
}

// Called every frame
void ALobbyPreviewCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALobbyPreviewCharacter::SetCharacterByData(const FPlayerCharacter& Data)
{
	UDataTable* CMSTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::CharacterMeshSet);
	FCharacterMeshSet* CMSData = CMSTable->FindRow<FCharacterMeshSet>(FROM_INT(Data.iCharMeshSetId), TEXT("FCharacterMeshSet"));
	if(!CMSData) return false;

	UDataTable* MLSTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::MeshList);
	FMeshList* BodyData = MLSTable->FindRow<FMeshList>(FROM_INT(CMSData->iBodyMeshId), TEXT("FMeshList"));
	if(!BodyData) return false;

	USkeletalMesh* FindBodyMesh = UAssetUtilsLibrary::GetSkeletalMeshFromPath(BodyData->sMeshPath);
	if(!FindBodyMesh) return false;
	BodyMesh->SetSkeletalMesh(FindBodyMesh);

	UDataTable* ASTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::AnimationSet);
	FAnimationSet* AnimData = ASTable->FindRow<FAnimationSet>(FROM_INT(Data.iCharAniSetId), TEXT("FAnimationSetList"));
	if(!AnimData) return false;

	UAnimSequence* FindAnim = UAssetUtilsLibrary::GetAnimSequenceFromPath(AnimData->sIdleAniPath);
	if(!FindAnim) return false;	
	BodyMesh->PlayAnimation(FindAnim, true);

	FMeshList* WeaponData = nullptr;
	if(CMSData->iWeaponMeshId != 0) WeaponData = MLSTable->FindRow<FMeshList>(FROM_INT(CMSData->iWeaponMeshId), TEXT("FMeshList"));
		
	if(!WeaponData) return true;
	if(!CHECK_FREE(WeaponData->sMeshPath))
	{
		UMeshComponent* WeaponComponent = nullptr;
		FString ScriptType = UGlobalUtilsLibrary::GetScriptTypeByRefPath(WeaponData->sMeshPath);
		if(ScriptType.Equals(TEXT("StaticMesh")))
		{
			UStaticMesh* FindWeaponMesh = UAssetUtilsLibrary::GetStaticMeshFromPath(WeaponData->sMeshPath);
			WeaponMesh->SetStaticMesh(FindWeaponMesh);
			UGlobalUtilsLibrary::EnableComponent(WeaponMesh, true);
			WeaponComponent = WeaponMesh;
		}
		else if(ScriptType.Equals(TEXT("SkeletalMesh")))
		{
			USkeletalMesh* FindWeaponMesh = UAssetUtilsLibrary::GetSkeletalMeshFromPath(WeaponData->sMeshPath);
			WeaponDynamicMesh->SetSkeletalMesh(FindWeaponMesh);
			UGlobalUtilsLibrary::EnableComponent(WeaponDynamicMesh, true);
			WeaponComponent = WeaponDynamicMesh;
		}
		else
		{					
			
		}

		CMSData->iWeaponSocketId;
		UDataTable* SocketTable = UAssetUtilsLibrary::LoadDataTableByEnumKey(EDataTableKey::SocketList);
		FSocketList* SocketData = SocketTable->FindRow<FSocketList>(FROM_INT(CMSData->iWeaponSocketId), TEXT("FSocketList"));
		if(CHECK_FREE(SocketData->sSocketName))
		{
			NMT_MSG_ENSURE(0, "Socket Name Is FREE [%s][%s]", FROM_INT(CMSData->iWeaponSocketId), *SocketData->sSocketName);
			return true;
		}

		WeaponComponent->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, *SocketData->sSocketName);
	}

	else
	{
		WeaponMesh->SetStaticMesh(nullptr);
		WeaponDynamicMesh->SetSkeletalMesh(nullptr);
	}

	return true;

}

