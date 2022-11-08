// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */

class ABlasterCharacter;
class ABlasterGameState;
UCLASS()
class SHOOTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ABlasterGameMode();

	virtual void BeginPlay()override;

	void ReserveGameStart();

	void ReserveGameEnd();

	void ReserverRestartGame();

	virtual void EliminatePlayer(ABlasterCharacter* eliminatedPlayer, AController* controller);

	void OnMatchStateSet()override;

private:
	UFUNCTION()
	void RespawnCharacter(ABlasterCharacter* character, AController* controller);


private:

	ABlasterGameState* _gameState;

	UPROPERTY(EditAnywhere)
	float _respawnTime=3.f;

	UPROPERTY(EditAnywhere)
		float _totalGameTime= 600.f;

	UPROPERTY(EditAnywhere)
		float _waitingTime = 20.f;

	UPROPERTY(EditAnywhere)
		float _postGameWaitingTime = 20.f;

};
