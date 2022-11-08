// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstanceSubsystem.generated.h"




/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiOnCreateSessionComplete, bool, wasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& sessionResults, bool wasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiOnJoinSessionsComplete, EOnJoinSessionCompleteResult::Type result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiOnDestroySessionComplete, bool, wasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiOnStartSessionComplete, bool, wasSuccessful);


UCLASS()
class MULTIPLAYERSESSION_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMyGameInstanceSubsystem();

	void CreateGameSession(int32 numPublicConnection,FString matchType);
	void FindSession(int32 maxSearchResults);
	void JoinGameSession(const FOnlineSessionSearchResult& sessionResult);

	void DestroySession();
	void StartSession();

	FMultiOnCreateSessionComplete _onCreateSessionComplete;
	FMultiOnFindSessionsComplete _onFindSessionComplete;
	FMultiOnJoinSessionsComplete _onJoinSessionComplete;
	FMultiOnStartSessionComplete _onStartSessionComplete;
	FMultiOnDestroySessionComplete _onDestroySessionComplete;


	IOnlineSessionPtr GetOnlineSessionInterface() { return _onlineSessionInterface; }

protected:
	void OnCreateSessionComplete(FName sessionName, bool wasSuccessful);
	void OnFindSessionComplete(bool wasSuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful);
	void OnStartSessionComplete(FName sessionName, bool wasSuccessful);



private:
	IOnlineSessionPtr _onlineSessionInterface;

	TSharedPtr<FOnlineSessionSettings> _lastCreationSetting;

	TSharedPtr<FOnlineSessionSearch> _sessionSearch;

	bool _pendingDestroy = false;

	////////////////////////////

	FOnCreateSessionCompleteDelegate _createSessionCompleteDelegate;
	FDelegateHandle _createSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate _findSessionCompleteDelegate;
	FDelegateHandle _findSessionCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate _joinSessionCompleteDelegate;
	FDelegateHandle _joinSessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate _startSessionCompleteDelegate;
	FDelegateHandle _startSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate _destroySessionCompleDelegate;
	FDelegateHandle _destroySessionCompleDelegateHandle;

};
