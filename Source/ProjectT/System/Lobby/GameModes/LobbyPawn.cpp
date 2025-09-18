
#include "LobbyPawn.h"
#include "ProjectT/System/Core/Components/CoreCameraComponent.h"


ALobbyPawn::ALobbyPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	LobbyPawnCamera = CreateDefaultSubobject<UCoreCameraComponent>(TEXT("LobbyPawnCamera"));
	LobbyPawnCamera->SetupAttachment(RootComponent);
}

void ALobbyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALobbyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

