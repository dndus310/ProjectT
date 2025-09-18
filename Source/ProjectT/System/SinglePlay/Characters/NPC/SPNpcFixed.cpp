// Fill out your copyright notice in the Description page of Project Settings.


#include "SPNpcFixed.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectT/System/Core/Characters/NPC/Animations/NpcFixedAnimInstance.h"
#include "ProjectT/System/Core/Characters/Player/CorePlayerCharacter.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/SinglePlay/Components/SPNpcStatusComponent.h"


ASPNpcFixed::ASPNpcFixed() :
	HeadPitchAngle(-37.f),
	DetectParameterName(TEXT("Detect"))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Fixed"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(UNpcFixedAnimInstance::StaticClass());
	
	DetectCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectCollisionComponent"));
	DetectSpotMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectSpotMeshComponent"));
	DetectConeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectConeMeshComponent"));
	BoundingBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundingBoxComponent"));
	NMT_CHECKF(DetectCollisionComponent && DetectSpotMeshComponent && DetectConeMeshComponent && BoundingBoxComponent)
	
	DetectCollisionComponent->SetupAttachment(RootComponent);
	DetectCollisionComponent->SetCollisionProfileName(TEXT("InteractionRange"));
	DetectCollisionComponent->SetSphereRadius(76.f);
	DetectCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASPNpcFixed::DetectBeginOverlap);
	DetectCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASPNpcFixed::DetectEndOverlap);
	
	DetectSpotMeshComponent->SetupAttachment(DetectCollisionComponent);
	DetectSpotMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DetectConeMeshComponent->SetupAttachment(SpringArmComponent);
	DetectConeMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DetectConeMeshComponent->SetRelativeLocation(FVector(140.0, 0, 8.0));
	DetectConeMeshComponent->SetRelativeRotation(FRotator(90.0, 0, 0));
	DetectConeMeshComponent->SetRelativeScale3D(FVector(1.25, 2.0, 2.5));

	BoundingBoxComponent->SetupAttachment(RootComponent);
	BoundingBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundingBoxComponent->SetBoxExtent(FVector(200.f, 150.f, 120.f));
	GetMesh()->SetupAttachment(BoundingBoxComponent);
	GetMesh()->bUseAttachParentBound = true;
	
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->SetComponentTickEnabled(false);
	WeaponDynamicComponent->SetComponentTickEnabled(false);
	WeaponStaticComponent->SetComponentTickEnabled(false);
	AccessoryMeshComponent->SetComponentTickEnabled(false);
}

void ASPNpcFixed::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASPNpcFixed::SetSpringArmRotation()
{
	FRotator ArmRotator = SpringArmComponent->GetComponentRotation();
	SpringArmComponent->SetWorldRotation(UKismetMathLibrary::MakeRotator(ArmRotator.Roll, ArmRotator.Pitch, GetMesh()->GetSocketRotation(EyeSocketName).Yaw));
}

void ASPNpcFixed::BeginPlay()
{
	Super::BeginPlay();
}

void ASPNpcFixed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPNpcFixed::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

UBlackboardData* ASPNpcFixed::GetBlackboardData_Implementation()
{
	return Blackboard;
}

UBehaviorTree* ASPNpcFixed::GetBehaviorTree_Implementation()
{
	return BehaviorTree;
}

void ASPNpcFixed::StartDisableNpc()
{
	Super::StartDisableNpc();

	SetVisionMode(EVisionMode::None);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	if(bEnableRespawn)
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASPNpcFixed::Respawn, RespawnTime, false);
}

void ASPNpcFixed::CompleteDisableNpc()
{
	Super::CompleteDisableNpc();
}

void ASPNpcFixed::ApplyDamageEvent_Implementation(AActor* InOccurActor, const FVector& InOriginLocation, const float InDamage, const TArray<FEffect>& InSkillEffects)
{
	if(NpcStatusComponent->ApplyDamage(InDamage))
	{
		ChangeNpcState(ENpcState::Dead);
	}
	else
	{
		for(FEffect EffectType : InSkillEffects)
		{
			EEffectSubTypes EffectSubType = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(EffectType.sEffect);
			switch(EffectSubType)
			{
				case EEffectSubTypes::None:
				{
					ChangeNpcState(ENpcState::Chase);
					break;
				}
				case EEffectSubTypes::Hitback:
				{
					ChangeNpcState(ENpcState::Hit);
					break;
				}
				case EEffectSubTypes::Jumpback:
				{
					ChangeNpcState(ENpcState::Hit);
					break;
				}
				case EEffectSubTypes::Unrecog:
				{
					SetUnDetectTarget(true);
					break;
				}
				case EEffectSubTypes::Slow: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Slow")
					break;
				case EEffectSubTypes::Bind: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Bind")
					break;
				case EEffectSubTypes::Fast: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Fast")
					break;
				case EEffectSubTypes::CogImmune: NOT_IMPLEMENTED_MSG("EEffectSubTypes::CogImmune")
					break;
				case EEffectSubTypes::Stun: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Stun")
					break;
				case EEffectSubTypes::Surge: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Surge")
					break;
				case EEffectSubTypes::HitbackImmune: NOT_IMPLEMENTED_MSG("EEffectSubTypes::HitbackImmune")
					break;
				case EEffectSubTypes::Heal: NOT_IMPLEMENTED_MSG("EEffectSubTypes::Heal")
					break;
				default: NMT_LOG("Not Valid")
			}
		}
	}
}

void ASPNpcFixed::ApplyRecoveryEvent_Implementation(const TArray<FEffect>& InReleaseEffects)
{
	for(FEffect LocCurEffect : InReleaseEffects)
	{
		EEffectSubTypes LoCurReleaseEffect = UGlobalUtilsLibrary::GetStringToEnum<EEffectSubTypes>(LocCurEffect.sEffect);
		switch(LoCurReleaseEffect)
		{
			case EEffectSubTypes::Unrecog: 
			{
				SetUnDetectTarget(false);
				
				TArray<AActor*> OverlappedActors;
				DetectCollisionComponent->GetOverlappingActors(OverlappedActors);
				if(!OverlappedActors.IsEmpty())
				{
					for(AActor* OverlappedActor : OverlappedActors)
					{
						if(ACorePlayerCharacter* Player = Cast<ACorePlayerCharacter>(OverlappedActor))
						{
							UpdateDetectState(true, Player);
							break;
						}
					}
				}
				break;
			}
			case EEffectSubTypes::CogImmune: break;
			case EEffectSubTypes::Surge: break;
			case EEffectSubTypes::Bind: break;
			case EEffectSubTypes::Fast: break;
			case EEffectSubTypes::Slow: break;
			case EEffectSubTypes::Hitback: break;
			case EEffectSubTypes::Jumpback:  break;
			case EEffectSubTypes::None: break;
			case EEffectSubTypes::Stun: break;
			case EEffectSubTypes::HitbackImmune: break;
			case EEffectSubTypes::Heal: break;
			default: NMT_LOG("No Release Effect Type");
		}
	}
}

void ASPNpcFixed::InitializeProperty()
{
	Super::InitializeProperty();

	FixedAnimInstance = Cast<UNpcFixedAnimInstance>(GetMesh()->GetAnimInstance());
	NMT_CHECKF(FixedAnimInstance);
	NpcAnimInstance->ForwardHeadRotation.Pitch = NpcAnimInstance->HeadRotation.Pitch = HeadPitchAngle;
	
	SetDetectSpotLocation();
	DetectConeMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true), EyeSocketName);
	DetectSpotDynamicMaterial = DetectSpotMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	DetectConeDynamicMaterial = DetectConeMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
}

void ASPNpcFixed::ActivateCharacter(const bool InbEnable)
{
	Super::ActivateCharacter(InbEnable);
}

#if WITH_EDITOR
void ASPNpcFixed::ApplyCharacterData(const FTableRowBase* RowData)
{
	Super::ApplyCharacterData(RowData);
}
#endif

void ASPNpcFixed::DetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACorePlayerCharacter* Player = Cast<ACorePlayerCharacter>(OtherActor))
	{
		if(OtherComp->ComponentHasTag("PlayerCapsule"))
			UpdateDetectState(true, Player);
	}
}

void ASPNpcFixed::DetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(Cast<ACorePlayerCharacter>(OtherActor) && OtherComp->ComponentHasTag("PlayerCapsule"))
	{
		UpdateDetectState(false, nullptr);
	}
}

void ASPNpcFixed::SetDetectSpotLocation()
{
	UWorld* World = UGlobalUtilsLibrary::GetValidWorld(this);
	NMT_CHECKF(World)

	FHitResult HitResult;
	FVector EyeLocation = GetMesh()->GetSocketLocation(EyeSocketName);
	FRotator EyeRotation = GetActorRotation() + FRotator(HeadPitchAngle, 0.0, 0.0);
	FVector Dir =  EyeRotation.Vector().GetSafeNormal();
	FVector EndLocation = EyeLocation + Dir * 1500.f;
	bool bHitGround = World->LineTraceSingleByObjectType(HitResult, EyeLocation, EndLocation, ECC_GameTraceChannel5, FCollisionQueryParams());	
	if(bHitGround)
	{
		DetectCollisionComponent->SetWorldLocation(FVector(HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z + 1.f));
	}
	
	DetectCollisionComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
}

void ASPNpcFixed::SetDetectMaterial(const bool InbDetect) const
{
	DetectSpotDynamicMaterial->SetScalarParameterValue(DetectParameterName, InbDetect);
	DetectConeDynamicMaterial->SetScalarParameterValue(DetectParameterName, InbDetect);
}

void ASPNpcFixed::UpdateDetectState(const bool InbDetect, const class ACorePlayerCharacter* InTarget)
{
	if(InbDetect)
	{
		if(!InTarget)
			return;
		
		bool PlayerUnDetect;
		InTarget->GetUnDetectState(PlayerUnDetect);
		if(PlayerUnDetect || bUnDetect)
			return;
		
		SetDetectMaterial(true);
		Alert(InTarget->GetActorLocation());
	}
	else
	{
		SetDetectMaterial(false);
		bCanTriggerAlert = true;
	}
}



