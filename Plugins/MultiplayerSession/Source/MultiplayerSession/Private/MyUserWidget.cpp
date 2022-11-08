// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "Components/Button.h"
#include "MyGameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"

void UMyUserWidget::MenuSetup(int32 numPublicConnections, FString matchType, FString lobbyPath)
{
	_lobbyPath = FString::Printf(TEXT("%s?listen"), *lobbyPath);
	_numPublicConnections = numPublicConnections;
	_matchType = matchType;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* pc = world->GetFirstPlayerController();
		if (pc)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputModeData);
			pc->SetShowMouseCursor(true);
		}
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		_onlineSessionSubsystem = gameInstance->GetSubsystem<UMyGameInstanceSubsystem>();
	}

	if (_onlineSessionSubsystem)
	{
		_onlineSessionSubsystem->_onCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		_onlineSessionSubsystem->_onFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
		_onlineSessionSubsystem->_onJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		_onlineSessionSubsystem->_onStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		_onlineSessionSubsystem->_onDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	}
}

bool UMyUserWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	if (_hostButton)
	{
		_hostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (_joinButton)
	{
		_joinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMyUserWidget::OnLevelRemovedFromWorld(ULevel* level, UWorld* world)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(level, world);
}


void UMyUserWidget::OnCreateSession(bool wasSuccessful)
{
	if (!wasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString(TEXT("Failed to create Session")));
		_hostButton->SetIsEnabled(true);
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString(TEXT("Session Created Successfully")));
	}
	UWorld* world = GetWorld();
	if (world)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, *_lobbyPath);
		world->ServerTravel(FString(_lobbyPath));
	}
	
}

void UMyUserWidget::OnFindSession(const TArray<FOnlineSessionSearchResult>& sessionResults, bool wasSuccessful)
{
	
	if (!_onlineSessionSubsystem || !wasSuccessful || sessionResults.Num()<=0)
	{
		_joinButton->SetIsEnabled(true);
		return;
	}


	for (auto result : sessionResults)
	{
		FString value;
		result.Session.SessionSettings.Get(FName("MatchType"), value);
		if (value.Equals(_matchType))
		{
			_onlineSessionSubsystem->JoinGameSession(result);
			return;
		}
	}
	
}

void UMyUserWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, TEXT("join session on "));

	IOnlineSessionPtr sessionInterface = _onlineSessionSubsystem->GetOnlineSessionInterface();

	if (!sessionInterface.IsValid() || ! (result == EOnJoinSessionCompleteResult::Success) )
	{
		_joinButton->SetIsEnabled(true);
		return;
	}
	
	FString address;
	sessionInterface->GetResolvedConnectString(NAME_GameSession, address);
	APlayerController* pc = GetGameInstance()->GetFirstLocalPlayerController();

	if (pc)
	{
		pc->ClientTravel(address, ETravelType::TRAVEL_Absolute);
	}


}

void UMyUserWidget::OnDestroySession(bool wasSuccessful)
{

}

void UMyUserWidget::OnStartSession(bool wasSuccessful)
{

}



void UMyUserWidget::HostButtonClicked()
{
	_hostButton->SetIsEnabled(false);
	if (_onlineSessionSubsystem)
	{	
		_onlineSessionSubsystem->CreateGameSession(_numPublicConnections,_matchType);
	}

}

void UMyUserWidget::JoinButtonClicked()
{
	_joinButton->SetIsEnabled(false);
	if (_onlineSessionSubsystem)
	{
		_onlineSessionSubsystem->FindSession(10000);

		//_onlineSessionSubsystem->JoinGameSession();
	}

}

void UMyUserWidget::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if(world)
	{
		APlayerController* pc = world->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameOnly inputMode;
			pc->SetInputMode(inputMode);
			pc->SetShowMouseCursor(false);
		}
	}
}
