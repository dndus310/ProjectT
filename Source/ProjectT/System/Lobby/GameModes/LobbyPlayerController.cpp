// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	bShowMouseCursor = true;
}

void ALobbyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);	
	InitializePlayerScreen();
}
