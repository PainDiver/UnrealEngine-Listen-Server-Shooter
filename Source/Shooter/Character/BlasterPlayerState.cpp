// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterController.h"
#include "../HUD/CharacterHUD.h"
#include "BlasterCharacter.h"

void ABlasterPlayerState::AddScore(ABlasterCharacter* killed, ABlasterCharacter* myCharacter, float score)
{
	if (killed == myCharacter)
	{
		return;
	}

	SetScore(GetScore() + score);
	
	if (myCharacter->IsLocallyControlled())
	{
		APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ABlasterPlayerState* state = nullptr;
		ACharacterHUD* hud = nullptr;
		if (controller)
		{
			state = controller->GetPlayerState<ABlasterPlayerState>();
			hud = Cast<ACharacterHUD>(controller->GetHUD());
		}

		if (hud && state) //server hud update
		{
			hud->UpdateStatusKill(state->GetScore());
		}
	}

}

void ABlasterPlayerState::AddDeath(ABlasterCharacter* killed)
{
	_death++;

	if (killed->IsLocallyControlled())
	{
		APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ABlasterPlayerState* state = nullptr;
		ACharacterHUD* hud = nullptr;
		if (controller)
		{
			state = controller->GetPlayerState<ABlasterPlayerState>();
			hud = Cast<ACharacterHUD>(controller->GetHUD());
		}

		if (hud && state) //server hud update
		{
			hud->UpdateStatusDeath(state->GetDeath());
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	// this will be fired in autonomously, so it can get client controller from UGameplayStatics
	APlayerController * controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABlasterPlayerState* state = nullptr;
	ACharacterHUD* hud = nullptr;
	if (controller)
	{
		state = controller->GetPlayerState<ABlasterPlayerState>();
		hud = Cast<ACharacterHUD>(controller->GetHUD());
	}

	if (hud && state) //server hud update
	{
		hud->UpdateStatusKill(state->GetScore());
	}
}

void ABlasterPlayerState::OnRep_Death()
{
	Super::OnRep_Score();

	// this will be fired in autonomously, so it can get client controller from UGameplayStatics
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABlasterPlayerState* state=nullptr;
	ACharacterHUD* hud=nullptr;
	if (controller)
	{
		state = controller->GetPlayerState<ABlasterPlayerState>();
		hud = Cast<ACharacterHUD>(controller->GetHUD());
	}

	if (hud && state) //server hud update
	{
		hud->UpdateStatusDeath(state->GetDeath());
	}
}

void ABlasterPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass,_death,COND_OwnerOnly);

}
