// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "VisionConeComponent.generated.h"

USTRUCT()
struct FSightTraceInfo
{
	GENERATED_BODY()
	
	FSightTraceInfo() :
		Point(FVector::ZeroVector),
		Range(0.f),
		Angle(0.f)
	{}

	FSightTraceInfo(const FVector& InPoint, const float InRange, const float InAngle) :
		Point(InPoint),
		Range(InRange),
		Angle(InAngle)
	{}

	UPROPERTY()
	FVector Point;

	UPROPERTY()
	float Range;

	UPROPERTY()
	float Angle;
};

USTRUCT()
struct FMeshGeometryData
{
	GENERATED_BODY()

	FMeshGeometryData() :
		VertexCount(0),
		IndexCount(0)
	{}
	
	FMeshGeometryData(int32 InVertexCount, int32 InIndexCount) :
		VertexCount(InVertexCount),
		IndexCount(InIndexCount)
	{}

	UPROPERTY()
	int32 VertexCount;

	UPROPERTY()
	int32 IndexCount;
    
	UPROPERTY()
	TArray<FVector> Vertices;
    
	UPROPERTY()
	TArray<int32> Indices;
    
	UPROPERTY()
	TArray<FVector> Normals;
    
	UPROPERTY()
	TArray<FVector2D> UV;
    
	UPROPERTY()
	TArray<FLinearColor> VertexColors;
    
	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;
};

UCLASS(ClassGroup = Rendering, meta = (BlueprintSpawnableComponent))
class PROJECTT_API UVisionConeComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:    
    UVisionConeComponent(const FObjectInitializer& ObjectInitializer);
    
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
    UFUNCTION(BlueprintCallable)
    void ToggleDraw(const bool InbDraw);

	FORCEINLINE bool IsDrawing() const { return bDrawing; }
	FORCEINLINE void SetAllowDrawing(const bool InbAllowDrawing) { bAllowDraw = InbAllowDrawing; }
	bool IsAllowedDrawing() const;
	void SetupSightSettings(const float InSightRange, const float InSightAngle);
	void UpdateVisionColor(const bool InbHit);
	void UpdateVision(const FRotator& InViewRotation);
	void UpdateVisionGauge(const float InGauge) const;
	
private:
	UFUNCTION()
	void EnableVision();

	UFUNCTION()
	void DisableVision();
	
    void InitializeMeshGeometry();
    void DrawLineOfSight(const FRotator& InViewRotation);
    void UpdateMaterialState(const FLinearColor& InColor);
    FSightTraceInfo GetSightTraceInfo(float InAngle) const;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight")
    uint8 bDebugIsEnabled : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight")
	uint8 bUpdateEveryFrame : 1;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight")
    float SightRange;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight", meta = (ClampMin  = 0, ClampMax = 360))
    float SightAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight")
    int32 TriangleCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight")
	TEnumAsByte<ECollisionChannel> VisionRangeTraceChannel;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight")
    FVector TraceStartLocation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight|MeshInfo")
    UMaterialInterface* VisionMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight|MeshInfo")
    FName VisionParamName; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight|MeshInfo")
	FName VisionGaugeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight|MeshInfo")
	FName VisionRangeName;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight|MeshInfo")
    FLinearColor DefaultColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight|MeshInfo")
    FLinearColor HitColor;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight|MeshInfo")
	float MeshOffset;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight", meta = (AllowprivateAccess = "true"))
	uint8 bAllowDraw : 1;
	
	UPROPERTY(VisibleAnywhere)
	uint8 bDrawing : 1;
	
    UPROPERTY()
    float DivideAngleSize;
	
	UPROPERTY()
	FMeshGeometryData MeshGeometryData;

    UPROPERTY()
    TArray<FVector> PrevViewPoints;

    UPROPERTY()
    TObjectPtr<class UMaterialInstanceDynamic> VisionDynamicMaterial;

    UPROPERTY()
    FLinearColor CurrentColor;

	UPROPERTY()
	TEnumAsByte<ETraceTypeQuery> VisionRangeTypeQuery;

	uint8 bUpdatedOnce;
};

