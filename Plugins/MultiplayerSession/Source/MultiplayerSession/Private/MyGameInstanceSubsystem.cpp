// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMyGameInstanceSubsystem::UMyGameInstanceSubsystem()
{
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem)
	{
		_onlineSessionInterface = onlineSubsystem->GetSessionInterface();
		_createSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
		_findSessionCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete);
		_joinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete);
		_startSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete);
		_destroySessionCompleDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete);
	}
}

void UMyGameInstanceSubsystem::CreateGameSession(int32 numPublicConnection, FString matchType)
{
	if (!_onlineSessionInterface.IsValid())
	{
		_onCreateSessionComplete.Broadcast(false);
		return;
	}

	if (_pendingDestroy)
	{
		return;
	}

	_lastCreationSetting = MakeShareable(new FOnlineSessionSettings());
	_lastCreationSetting->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL"? true: false;
	_lastCreationSetting->NumPublicConnections = numPublicConnection;
	_lastCreationSetting->bAllowJoinViaPresence = true;
	_lastCreationSetting->bAllowJoinViaPresence = true;
	_lastCreationSetting->bShouldAdvertise = true;
	_lastCreationSetting->bUsesPresence = true;
	_lastCreationSetting->bUseLobbiesIfAvailable = true;
	_lastCreationSetting->BuildUniqueId = 1;
	_lastCreationSetting->Set(FName("MatchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	auto existingSession = _onlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr)
	{
		
		_pendingDestroy = true;
		DestroySession();
		return;
	}

	_createSessionCompleteDelegateHandle = _onlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(_createSessionCompleteDelegate);

	ULocalPlayer* player = GetWorld()->GetFirstLocalPlayerFromController();
	if (!_onlineSessionInterface->CreateSession(*(player->GetPreferredUniqueNetId()), NAME_GameSession, *_lastCreationSetting))
	{
		_onlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(_createSessionCompleteDelegateHandle);
		_onCreateSessionComplete.Broadcast(false);
	}

}

void UMyGameInstanceSubsystem::FindSession(int32 maxSearchResults)
{
	if (!_onlineSessionInterface.IsValid())
	{
		_onFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	_findSessionCompleteDelegateHandle = _onlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(_findSessionCompleteDelegate);
	
	_sessionSearch = MakeShareable(new FOnlineSessionSearch());
	_sessionSearch->MaxSearchResults = maxSearchResults;
	_sessionSearch->bIsLanQuery = false;
	_sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* player = GetWorld()->GetFirstLocalPlayerFromController();
	if (!_onlineSessionInterface->FindSessions(*(player->GetPreferredUniqueNetId()), _sessionSearch.ToSharedRef()))
	{
		_onlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(_findSessionCompleteDelegateHandle);
		_onFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}

}

void UMyGameInstanceSubsystem::JoinGameSession(const FOnlineSessionSearchResult& sessionResult)
{
	if (!_onlineSessionInterface.IsValid())
	{
		_onJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	_joinSessionCompleteDelegateHandle =_onlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(_joinSessionCompleteDelegate);

	const ULocalPlayer* player = GetWorld()->GetFirstLocalPlayerFromController();
	if (!_onlineSessionInterface->JoinSession(*(player->GetPreferredUniqueNetId()), NAME_GameSession, sessionResult))
	{
		_onlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(_joinSessionCompleteDelegateHandle);
		
		_onJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMyGameInstanceSubsystem::DestroySession()
{
	if (!_onlineSessionInterface.IsValid())
	{
		_onDestroySessionComplete.Broadcast(false);
		return;
	}

	_destroySessionCompleDelegateHandle = _onlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(_destroySessionCompleDelegate);
	if (!_onlineSessionInterface->DestroySession(NAME_GameSession))
	{
		_onlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(_destroySessionCompleDelegateHandle);
		_onDestroySessionComplete.Broadcast(false);
	}
}

void UMyGameInstanceSubsystem::StartSession()
{

} 


void UMyGameInstanceSubsystem::OnCreateSessionComplete(FName sessionName, bool wasSuccessful)
{
	if (_onlineSessionInterface.IsValid())
	{
		_onlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(_createSessionCompleteDelegateHandle);
	}

	_onCreateSessionComplete.Broadcast(wasSuccessful);
}


void UMyGameInstanceSubsystem::OnFindSessionComplete(bool wasSuccessful)
{
	if (!_onlineSessionInterface.IsValid())
	{
		_onlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(_findSessionCompleteDelegateHandle);
	}

	if (_sessionSearch->SearchResults.Num() <= 0)
	{
		_onFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	_onFindSessionComplete.Broadcast(_sessionSearch->SearchResults, wasSuccessful);
}


void UMyGameInstanceSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (_onlineSessionInterface.IsValid())
	{
		_onlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(_joinSessionCompleteDelegateHandle);
	}

	_onJoinSessionComplete.Broadcast(result);

}

void UMyGameInstanceSubsystem::OnDestroySessionComplete(FName sessionName, bool wasSuccessful)
{
	if (_onlineSessionInterface.IsValid())
	{
		_onlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(_destroySessionCompleDelegateHandle);
	}

	if (wasSuccessful)
	{
		FString value;
		_lastCreationSetting->Get(FName("MatchType"), value);
		_pendingDestroy = false;
		CreateGameSession(_lastCreationSetting->NumPublicConnections,value);
	}
	_onDestroySessionComplete.Broadcast(wasSuccessful);
	
}

void UMyGameInstanceSubsystem::OnStartSessionComplete(FName sessionName, bool wasSuccessful)
{


}
