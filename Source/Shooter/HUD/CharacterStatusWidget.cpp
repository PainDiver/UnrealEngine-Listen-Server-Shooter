// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UCharacterStatusWidget::SetHPPercent(float hp, float maxHP)
{
	if (_hpBar)
	{
		_hpBar->SetPercent(hp / maxHP);
	}
}

void UCharacterStatusWidget::SetKill(int kill)
{
	if (_kill)
	{
		_kill->SetText(FText::FromString(FString::Printf(TEXT("%d"), kill)));
	}
}

void UCharacterStatusWidget::SetDeath(int death)
{
	if (_death)
	{
		_death->SetText(FText::FromString(FString::Printf(TEXT("%d"), death)));
	}
}
