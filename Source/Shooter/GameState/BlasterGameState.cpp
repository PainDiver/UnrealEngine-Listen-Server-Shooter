// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "../Character/BlasterController.h"
#include "../HUD/CharacterHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../GameMode/BlasterGameMode.h"
#include "Net/UnrealNetwork.h"
#include "../Character/BlasterPlayerState.h"

void ABlasterGameState::BeginPlay()
{
	Super::BeginPlay();

}

void ABlasterGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABlasterController* blasterController = Cast<ABlasterController>(controller);

	if (!blasterController)
	{
		return;
	}

	ACharacterHUD* hud = Cast<ACharacterHUD>(blasterController->GetHUD());
	if (!hud)
	{
		return;
	}

	FName matchState = GetMatchState();
	if (matchState == MatchState::WaitingToStart)
	{
		hud->SetAllWidgetVisibleState(ESlateVisibility::Hidden);
		hud->SetWidgetVisibleState(EWidgetType::EWT_GAMESTATE, ESlateVisibility::Visible);
	}
	else if (matchState == MatchState::InProgress)
	{
		hud->SetAllWidgetVisibleState(ESlateVisibility::Visible);
	}
	else if (matchState == MatchState::WaitingPostMatch)
	{
		hud->SetWidgetVisibleState(EWidgetType::EWT_STATUS, ESlateVisibility::Hidden);
		hud->SetWidgetVisibleState(EWidgetType::EWT_WEAPON_STATUS, ESlateVisibility::Hidden);

	}
	else if (matchState == MatchState::LeavingMap)
	{
		hud->SetAllWidgetVisibleState(ESlateVisibility::Hidden);
	}		
	
	
}

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass,_totalGameTime);
	DOREPLIFETIME(ThisClass,_waitingTime);
	DOREPLIFETIME(ThisClass, _postWaitingTime);
}

