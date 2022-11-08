// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterController.h"
#include "../HUD/CharacterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "../GameState/BlasterGameState.h"
#include "BlasterPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABlasterController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (aPawn->IsLocallyControlled())
	{
		_hud = GetHUD() ? Cast<ACharacterHUD>(GetHUD()) : nullptr;
		if (_hud)
		{
			_hud->UpdateStatusHP();
		}
	}
	_gameState =GetWorld()->GetGameState<ABlasterGameState>();
}

void ABlasterController::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle tempTimer;
	FTimerDelegate dele;
	dele.BindUObject(this, &ThisClass::UpdateTime);
	GetWorldTimerManager().SetTimer(tempTimer,dele, 0.125f, true);
}

void ABlasterController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (!HasAuthority())
		return;

	_syncTimer += deltaTime;
	if (_syncTimer > _timeSyncfreq)
	{
		SyncWorldTime(GetWorld()->GetTimeSeconds());
		_syncTimer = 0.f;

		ABlasterPlayerState* state = GetPlayerState<ABlasterPlayerState>();
		if (state)
		{
			SetPing(state->GetPing()*4.f);
		}
	}

}

void ABlasterController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	SyncWorldTime(GetWorld()->GetTimeSeconds());
}


void ABlasterController::SyncWorldTime_Implementation(float clientRequestTime)
{
	float serverTime = GetWorld()->GetTimeSeconds();
	SyncWorldTime_C(serverTime,clientRequestTime);
}

void ABlasterController::SyncWorldTime_C_Implementation(float serverTime, float clientRequestTime)
{
	float currentClientTime = GetWorld()->GetTimeSeconds();
	
	float roundTripTime = currentClientTime - clientRequestTime;
	if (roundTripTime <0)
	{
		roundTripTime = 0;
	}

	// server time + server to client time
	_singleTripTime = roundTripTime * 0.5f;
	float currentServerTime = serverTime + _singleTripTime;

	_serverClientDelta = currentServerTime - currentClientTime;
}


float ABlasterController::GetServerTime()
{

	return GetWorld()->GetTimeSeconds() + _serverClientDelta + _ping/1000.f;
}

void ABlasterController::UpdateTime()
{
	if (!_gameState)
	{
		_gameState = GetWorld()->GetGameState<ABlasterGameState>();
	}

	if (!_hud)
	{
		_hud = GetHUD() ? Cast<ACharacterHUD>(GetHUD()) : nullptr;
	}
	
	if (_hud && _gameState)
	{
		FName matchState = _gameState->GetMatchState();
		if (matchState == MatchState::WaitingToStart)
		{
			_hud->PlayWidgetAnimation<UGameStateWidget>(EWidgetType::EWT_GAMESTATE, FString("Ticking"));
			_hud->UpdateAnnouncement("Game Starts In");
			_hud->UpdateTime(_gameState->GetWaitingGameTime() - GetServerTime());
		}
		else if (matchState == MatchState::InProgress)
		{
			_hud->UpdateAnnouncement("Time Left");
			_hud->UpdateTime(_gameState->GetWaitingGameTime() + _gameState->GetTotalGameTime() - GetServerTime());
		}
		else if (matchState == MatchState::WaitingPostMatch)
		{
			_hud->UpdateAnnouncement("this Game will be restarted in");
			_hud->UpdateTime(_gameState->GetWaitingGameTime() + _gameState->GetTotalGameTime() + _gameState->GetPostWaitingGameTime() - GetServerTime());
		}
	}
}


void ABlasterController::SetPing_Implementation(float ping)
{
	_ping = ping;
}

void ABlasterController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, _ping, COND_OwnerOnly);
	

}

