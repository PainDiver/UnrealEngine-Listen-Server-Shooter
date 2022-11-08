// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponStatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UWeaponStatusWidget::SetCarriedAmmo(int ammo)
{
	if (_carriedAmmo)
	{
		_carriedAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), ammo)));
	}
}

void UWeaponStatusWidget::SetCurrentAmmo(int ammo)
{
	if (_currentAmmo)
	{
		_currentAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), ammo)));
	}
}

void UWeaponStatusWidget::SetWeaponName(FString name)
{
	if (_weaponName)
	{
		_weaponName->SetText(FText::FromString(FString::Printf(TEXT("%s"), *name)));
	}
}
