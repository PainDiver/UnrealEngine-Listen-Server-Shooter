// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComfyUserWidget.h"
#include "../Utils/GlobalStruct.h"
#include "GameStateWidget.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class SHOOTER_API UGameStateWidget : public UComfyUserWidget
{
	GENERATED_BODY()


	friend class ACharacterHUD;

	void SetTime(float totalTime);

	void SetAnnouncement(FString announcement);

private:

	UPROPERTY(meta = (BindWidget))
		UTextBlock* _time;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* _announcement;
};
