// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameState.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;

}


void ALobbyGameMode::PostLogin(APlayerController* newController)
{
	Super::PostLogin(newController);

	int32 playerNum = GameState.Get()->PlayerArray.Num();

	if (playerNum == 2)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}
	
}