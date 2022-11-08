// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */

class ABlasterCharacter;

UCLASS()
class SHOOTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:

	void AddScore(ABlasterCharacter* killed,ABlasterCharacter* myCharacter ,float score);

	void AddDeath(ABlasterCharacter* killed);


	virtual void OnRep_Score()override;

	UFUNCTION()
	void OnRep_Death();

	virtual void PostInitializeComponents() override;

	FORCEINLINE int GetDeath() { return _death; }


	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	UPROPERTY(replicatedUsing= OnRep_Death)
	int _death = 0;

};
