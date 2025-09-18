
#include "LobbyGameState.h"


ALobbyGameState::ALobbyGameState()	
{
	PrimaryActorTick.bCanEverTick = true;
}


void ALobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);	
}
