// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 * 
 */
namespace MatchState
{
	extern ENGINE_API const FName EnteringMap;			
	extern ENGINE_API const FName WaitingToStart;		
	extern ENGINE_API const FName InProgress;			
	extern ENGINE_API const FName WaitingPostMatch;		
	extern ENGINE_API const FName LeavingMap;			
	extern ENGINE_API const FName Aborted;				
}


UCLASS()
class SHOOTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:

	void BeginPlay()override;

	FORCEINLINE void SetTotalGameTime(float totalGameTime) {  _totalGameTime = totalGameTime; }
	FORCEINLINE float GetTotalGameTime() { return _totalGameTime; }

	FORCEINLINE void SetWaitingGameTime(float waitingTime) { _waitingTime = waitingTime; }
	FORCEINLINE float GetWaitingGameTime() { return _waitingTime; }

	FORCEINLINE void SetPostWaitingGameTime(float postWaitingTime) { _postWaitingTime = postWaitingTime; }
	FORCEINLINE float GetPostWaitingGameTime() { return _postWaitingTime; }


	void OnRep_MatchState()override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

private:
	UPROPERTY(replicated)
	float _totalGameTime;

	UPROPERTY(replicated)
	float _waitingTime;

	UPROPERTY(replicated)
	float _postWaitingTime;

};
