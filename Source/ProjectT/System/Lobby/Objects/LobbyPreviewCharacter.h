
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectT/Data/Gen/GenerateStructPlayerCharacter.h"
#include "LobbyPreviewCharacter.generated.h"

UCLASS()
class PROJECTT_API ALobbyPreviewCharacter : public AActor
{
	GENERATED_BODY()
	
public:		
	ALobbyPreviewCharacter();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool SetCharacterByData(const FPlayerCharacter& Data);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> WeaponDynamicMesh;

};
