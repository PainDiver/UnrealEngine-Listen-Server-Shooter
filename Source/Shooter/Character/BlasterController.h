// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterController.generated.h"

/**
 * 
 */
class ACharacterHUD;
class ABlasterGameState;
UCLASS()
class SHOOTER_API ABlasterController : public APlayerController
{
	GENERATED_BODY()
	

public: 
	virtual void OnPossess(APawn* aPawn)override;

	virtual void BeginPlay()override;

	void Tick(float deltaTime)override;

	void ReceivedPlayer()override;

	float GetServerTime();

	UFUNCTION()
	void UpdateTime();

	UFUNCTION(Server,Reliable)
	void SyncWorldTime(float clientRequestTime);

	UFUNCTION(Client, Reliable)
	void SyncWorldTime_C(float serverTime, float clientRequestTime);

	UFUNCTION(Server, Reliable)
		void SetPing(float ping);

	bool IsHighPing() { return _ping >= _pingThreshHold; }

	FORCEINLINE float GetSingleTripTime() { return _singleTripTime; }

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;



private:
	
	float _serverClientDelta =0.f;

	float _timeSyncfreq=5.f;

	float _syncTimer = 0.f;

	ACharacterHUD* _hud;

	ABlasterGameState* _gameState;

	UPROPERTY(replicated)
	float _ping;

	float _pingThreshHold = 70.f;

	float _singleTripTime;
};
