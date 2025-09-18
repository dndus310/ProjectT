// Fill out your copyright notice in the Description page of Project Settings.


#include "VisionConeComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "ProjectT/System/Core/Common/GlobalUtilsLibrary.h"
#include "ProjectT/System/Core/Interfaces/AISettings.h"
#include "ProjectT/System/Core/Managers/CoreCheatManager.h"

UVisionConeComponent::UVisionConeComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	bDebugIsEnabled(false),
	bUpdateEveryFrame(true),
	SightRange(500.f),
	SightAngle(90.f),
	TriangleCount(10),
	VisionRangeTraceChannel(ECC_GameTraceChannel2),	// NOTE : Vision
	TraceStartLocation(FVector::ZeroVector),
	VisionMaterial(nullptr),
	VisionParamName(TEXT("BaseColor")),
	VisionGaugeName(TEXT("Amount")),
	VisionRangeName(TEXT("MaxRange")),
	DefaultColor(FLinearColor(0.f, 1.f, 0.f, 0.3f)),
	HitColor(FLinearColor(1.f, 0.f, 0.f, 0.3f)),
	MeshOffset(50.f),
	bAllowDraw(false),
	bDrawing(false),
	bUpdatedOnce(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	BodyInstance.SetCollisionProfileName(TEXT("NoCollision"));
	BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCanEverAffectNavigation(false);
}

void UVisionConeComponent::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(VisionMaterial))
	{
		VisionDynamicMaterial = CreateAndSetMaterialInstanceDynamicFromMaterial(0, VisionMaterial);
	}

	InitializeMeshGeometry();
	VisionRangeTypeQuery = UEngineTypes::ConvertToTraceType(VisionRangeTraceChannel);
	bDrawing ? SetComponentTickEnabled(true) : SetComponentTickEnabled(false);

	VisionDynamicMaterial->SetScalarParameterValue(VisionRangeName, SightRange);
}

void UVisionConeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#if WITH_EDITOR
void UVisionConeComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitializeMeshGeometry();
}
#endif

void UVisionConeComponent::InitializeMeshGeometry()
{
	DivideAngleSize = SightAngle / TriangleCount;

	MeshGeometryData.VertexCount = TriangleCount + 2;
	MeshGeometryData.Vertices.SetNum(MeshGeometryData.VertexCount);
	MeshGeometryData.Vertices[0] = -FVector(0, 0, MeshOffset);

	MeshGeometryData.IndexCount = (MeshGeometryData.VertexCount - 2) * 3;
	MeshGeometryData.Indices.SetNum(MeshGeometryData.IndexCount);

	for(int32 Index = 0; Index < MeshGeometryData.Vertices.Num() - 1; ++Index)
	{
		if(Index < MeshGeometryData.VertexCount - 2)
		{
			MeshGeometryData.Indices[Index * 3 + 2] = 0;
			MeshGeometryData.Indices[Index * 3 + 1] = Index + 1;
			MeshGeometryData.Indices[Index * 3] = Index + 2;
		}
	}
}

void UVisionConeComponent::DrawLineOfSight(const FRotator& InViewRotation)
{
	TArray<FVector> ViewPoints;

	for(int32 Index = 0; Index < TriangleCount + 1; ++Index)
	{
		float Angle = InViewRotation.Yaw - (SightAngle / 2.0f) + (DivideAngleSize * Index);
		FSightTraceInfo SightTraceInfo = GetSightTraceInfo(Angle);
		ViewPoints.Add(SightTraceInfo.Point);
	}

	if(PrevViewPoints.Num() == ViewPoints.Num() && PrevViewPoints == ViewPoints)
		return;

	for(int32 Index = 0; Index < ViewPoints.Num(); ++Index)
	{
		MeshGeometryData.Vertices[Index + 1] = GetComponentTransform().InverseTransformPosition(ViewPoints[Index] - FVector(0, 0, MeshOffset));
	}

	if(GetProcMeshSection(0) == nullptr)
	{
		CreateMeshSection_LinearColor(0, MeshGeometryData.Vertices, MeshGeometryData.Indices, MeshGeometryData.Normals, MeshGeometryData.UV, MeshGeometryData.VertexColors, MeshGeometryData.Tangents, false);
	}
	else
	{
		UpdateMeshSection_LinearColor(0, MeshGeometryData.Vertices, MeshGeometryData.Normals, MeshGeometryData.UV, MeshGeometryData.VertexColors, MeshGeometryData.Tangents);
	}
	
	PrevViewPoints = ViewPoints;
}

void UVisionConeComponent::UpdateMaterialState(const FLinearColor& InColor)
{
	// if (!NMT_ENSURE(VisionDynamicMaterial))
	// 	return;

	if(VisionDynamicMaterial == nullptr)
		return;

	if(CurrentColor == InColor)
		return;

	VisionDynamicMaterial->SetVectorParameterValue(VisionParamName, InColor);
	CurrentColor = InColor;
}

FSightTraceInfo UVisionConeComponent::GetSightTraceInfo(float InAngle) const
{
	FHitResult OutHit;
	FVector Dir = FRotator(0.f, InAngle, 0.f).Vector();
	FVector Start;
	GetOwner()->GetClass()->ImplementsInterface(UAISettings::StaticClass()) ? Start = IAISettings::Execute_GetViewTransform(GetOwner()).GetLocation() : Start = GetComponentLocation();

	FVector End = (Dir * SightRange) + Start;

#if WITH_EDITOR
	EDrawDebugTrace::Type DrawDebugType = bDebugIsEnabled ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	TArray<AActor*> ActorsToIgnore;

	if(UKismetSystemLibrary::LineTraceSingle(this, Start, End, VisionRangeTypeQuery, false, ActorsToIgnore, DrawDebugType, OutHit, true))
	{
		return FSightTraceInfo(OutHit.Location, OutHit.Distance, InAngle);
	}
#else
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, VisionRangeTraceChannel))
	{
		return FSightTraceInfo(OutHit.Location, OutHit.Distance, InAngle);
	}
#endif


	return FSightTraceInfo(OutHit.TraceEnd, SightRange, InAngle);
}

void UVisionConeComponent::SetupSightSettings(const float InSightRange, const float InSightAngle)
{
	SightRange = InSightRange;
	SightAngle = InSightAngle;
}

void UVisionConeComponent::UpdateVisionColor(const bool InbHit)
{
	InbHit ? UpdateMaterialState(HitColor) : UpdateMaterialState(DefaultColor);
}

void UVisionConeComponent::UpdateVision(const FRotator& InViewRotation)
{
	if(bUpdateEveryFrame)
	{
		DrawLineOfSight(InViewRotation);
	}
	else
	{
		if(!bUpdatedOnce)
		{
			DrawLineOfSight(InViewRotation);
			bUpdatedOnce = true;
		}
	}
}

void UVisionConeComponent::UpdateVisionGauge(const float InGauge) const
{
	VisionDynamicMaterial->SetScalarParameterValue(VisionGaugeName, InGauge);
}

void UVisionConeComponent::ToggleDraw(const bool InbDraw)
{
	UCoreCheatManager* CCM = UGlobalUtilsLibrary::GetSharedCoreCheatManager(this);
	bool bDraw = InbDraw | CCM->GetDebugStatus().bDebugVision;
	if(IsMeshSectionVisible(0) != bDraw)
	{
		if(bDraw)
		{
			bAllowDraw = true;
			SetMeshSectionVisible(0, true);
			SetComponentTickEnabled(true);
		}
		else
		{
			bAllowDraw = false;
			SetMeshSectionVisible(0, false);
			SetComponentTickEnabled(false);
		}

		bDrawing = bDraw;
	}
}

bool UVisionConeComponent::IsAllowedDrawing() const
{
	return bAllowDraw;
}

void UVisionConeComponent::EnableVision()
{
	SetComponentTickEnabled(true);
	SetMeshSectionVisible(0, true);
}

void UVisionConeComponent::DisableVision()
{
	SetMeshSectionVisible(0, false);
	SetComponentTickEnabled(false);
}
