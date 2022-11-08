// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
class UButton;
class UMyGameInstanceSubsystem;
UCLASS()
class MULTIPLAYERSESSION_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 numPublicConnections =4 , FString matchType = FString(TEXT("FreeForAll")), FString lobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
	virtual bool Initialize()override;

	virtual void OnLevelRemovedFromWorld(ULevel* level, UWorld* world)override;


	UFUNCTION()
	void OnCreateSession(bool wasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& sessionResults, bool wasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type result);
	
	UFUNCTION()
	void OnDestroySession(bool wasSuccessful);
	
	UFUNCTION()
	void OnStartSession(bool wasSuccessful);


private:

	UPROPERTY(meta = (BindWidget))
	UButton* _hostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* _joinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

private:
	int32 _numPublicConnections = 4;
	FString _matchType{ TEXT("FreeForAll") };
	FString _lobbyPath{ TEXT("/Game/ThirdPerson/Maps/Lobby?listen") };

	UMyGameInstanceSubsystem* _onlineSessionSubsystem;

};
