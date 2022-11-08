// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateWidget.h"
#include "Components/TextBlock.h"

void UGameStateWidget::SetTime(float totalTime)
{
	int minutes = totalTime / 60;
	int seconds = totalTime - 60 * minutes;
	if (minutes < 0 || seconds < 0)
	{
		return;
	}
	if (_time)
	{
		_time->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), minutes, seconds)));
	}
}

void UGameStateWidget::SetAnnouncement(FString announcement)
{
	if (_announcement)
	{
		_announcement->SetText(FText::FromString(FString::Printf(TEXT("%s"), *announcement)));
	}
}
