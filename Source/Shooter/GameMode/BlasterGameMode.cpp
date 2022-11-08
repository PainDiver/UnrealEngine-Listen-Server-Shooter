// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "../Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "../GameState/BlasterGameState.h"


ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	_gameState = Cast<ABlasterGameState>(GetWorld()->GetGameState());
	_gameState->SetTotalGameTime(_totalGameTime);
	_gameState->SetWaitingGameTime(_waitingTime);

	ReserveGameStart();
}

void ABlasterGameMode::ReserveGameStart()
{
	FTimerHandle tempTimer;
	GetWorldTimerManager().SetTimer(tempTimer, FTimerDelegate::CreateLambda([&]()
		{
			SetMatchState(MatchState::InProgress);
			ReserveGameEnd();
		}), _waitingTime, false);
}

void ABlasterGameMode::ReserveGameEnd()
{
	FTimerHandle tempTimer;
	GetWorldTimerManager().SetTimer(tempTimer, FTimerDelegate::CreateLambda([&]()
		{
			SetMatchState(MatchState::WaitingPostMatch);
			ReserverRestartGame();
		}), _totalGameTime, false);
}

void ABlasterGameMode::ReserverRestartGame()
{
	FTimerHandle tempTimer;
	GetWorldTimerManager().SetTimer(tempTimer, FTimerDelegate::CreateLambda([&]()
		{
			RestartGame();
		}), _postGameWaitingTime, false);
}

void ABlasterGameMode::EliminatePlayer(ABlasterCharacter* eliminatedPlayer, AController* controller)
{
	FTimerHandle timer;
	FTimerDelegate respawnDelegate;
	respawnDelegate.BindUFunction(this, FName("RespawnCharacter"), eliminatedPlayer, controller);
	GetWorldTimerManager().SetTimer(timer,respawnDelegate,_respawnTime, false);
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	if (_gameState)
	{
		_gameState->SetMatchState(GetMatchState());
	}

	QuickDebug_S(GetMatchState().ToString());

}

void ABlasterGameMode::RespawnCharacter(ABlasterCharacter* character, AController* controller)
{
	if (character)
	{
		character->Reset();
		character->Destroy();
	}
	if (controller)
	{
		TArray<AActor*> playerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);
		if (playerStarts.Num() > 0)
		{
			int32 randIndex = FMath::RandRange(0, playerStarts.Num() - 1);
			playerStarts[randIndex];
			RestartPlayerAtPlayerStart(controller, playerStarts[randIndex]);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Controller to Spawn Character"));
		}
	}
}
