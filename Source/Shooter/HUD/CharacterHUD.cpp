// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"
#include "CharacterStatusWidget.h"
#include "Blueprint/UserWidget.h"
#include "../Character/BlasterCharacter.h"
#include "Components/TextBlock.h"
#include "WeaponStatusWidget.h"
#include "GameStateWidget.h"
#include "../GameState/BlasterGameState.h"

void ACharacterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (_crossHairPack.IsNull())
	{
		return;
	}

	DrawCrossHair();
}


void ACharacterHUD::SetCrossHairTexture(const FCrossHairPack& pack)
{
	_crossHairPack.SetCrossHairs(pack);
}

void ACharacterHUD::BeginPlay()
{
	Super::BeginPlay();
	
	_controller = GetOwningPlayerController();
	if (!_controller)
	{
		return;
	}
	

	if (_statusWidgetClass)
	{
		UCharacterStatusWidget* statusWidget = CreateWidget<UCharacterStatusWidget>(_controller, _statusWidgetClass);
		if (statusWidget)
		{
			statusWidget->AddToViewport();
			UpdateStatusHP();
			_widgets.Add(EWidgetType::EWT_STATUS, statusWidget);
		}
	}

	if (_weaponStatusWidgetClass)
	{
		UWeaponStatusWidget* weaponStatusWidget = CreateWidget<UWeaponStatusWidget>(_controller, _weaponStatusWidgetClass);
		if (weaponStatusWidget)
		{
			weaponStatusWidget->AddToViewport();
			_widgets.Add(EWidgetType::EWT_WEAPON_STATUS, weaponStatusWidget);
		}
	}
	
	if (_gameStateWidgetClass)
	{
		UGameStateWidget* gameStateWidget = CreateWidget<UGameStateWidget>(_controller, _gameStateWidgetClass);
		if (gameStateWidget)
		{
			gameStateWidget->AddToViewport();
			_widgets.Add(EWidgetType::EWT_GAMESTATE, gameStateWidget);
		}
	}
}

void ACharacterHUD::UpdateStatusHP()
{
	_controller = GetOwningPlayerController();
	if (_controller)
	{
		ABlasterCharacter* character = Cast<ABlasterCharacter>(_controller->GetCharacter());
		if (character && _widgets.Contains(EWidgetType::EWT_STATUS))
		{
			GetWidget<UCharacterStatusWidget>(EWidgetType::EWT_STATUS)->SetHPPercent(character->GetHP(),character->GetMaxHP());
		}
	}
}


void ACharacterHUD::UpdateStatusKill(int kill)
{
	if ( _widgets.Contains(EWidgetType::EWT_STATUS))
	{
		GetWidget<UCharacterStatusWidget>(EWidgetType::EWT_STATUS)->SetKill(kill);
	}

}

void ACharacterHUD::UpdateStatusDeath(int death)
{
	if (_widgets.Contains(EWidgetType::EWT_STATUS))
	{
		GetWidget<UCharacterStatusWidget>(EWidgetType::EWT_STATUS)->SetDeath(death);
	}
}

void ACharacterHUD::UpdateWeaponAmmo(int ammo)
{
	if ( _widgets.Contains(EWidgetType::EWT_WEAPON_STATUS))
	{
		GetWidget<UWeaponStatusWidget>(EWidgetType::EWT_WEAPON_STATUS)->SetCurrentAmmo(ammo);
	}
}

void ACharacterHUD::UpdateWeaponCarriedAmmo(int ammo)
{
	if (_widgets.Contains(EWidgetType::EWT_WEAPON_STATUS))
	{
		GetWidget<UWeaponStatusWidget>(EWidgetType::EWT_WEAPON_STATUS)->SetCarriedAmmo(ammo);
	}
}

void ACharacterHUD::UpdateWeaponName(FString name)
{
	if (_widgets.Contains(EWidgetType::EWT_WEAPON_STATUS))
	{
		GetWidget<UWeaponStatusWidget>(EWidgetType::EWT_WEAPON_STATUS)->SetWeaponName(name);
	}
}

void ACharacterHUD::UpdateTime(float totalTime)
{
	if ( _widgets.Contains(EWidgetType::EWT_GAMESTATE))
	{
		GetWidget<UGameStateWidget>(EWidgetType::EWT_GAMESTATE)->SetTime(totalTime);
	}
}

void ACharacterHUD::UpdateAnnouncement(FString announcement)
{
	if (_widgets.Contains(EWidgetType::EWT_GAMESTATE))
	{
		GetWidget<UGameStateWidget>(EWidgetType::EWT_GAMESTATE)->SetAnnouncement(announcement);
	}
}

void ACharacterHUD::SetWidgetVisibleState(EWidgetType widgetType,ESlateVisibility setting)
{
	if (_widgets.Contains(widgetType))
	{
		_widgets[widgetType]->SetVisibility(setting);
	}
}

void ACharacterHUD::SetAllWidgetVisibleState(ESlateVisibility setting)
{
	for (auto widget : _widgets)
	{
		widget.Value->SetVisibility(setting);
	}

}


void ACharacterHUD::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	
}

void ACharacterHUD::DrawCrossHair()
{

	FVector2D screenSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(screenSize);
	}

	FVector2D middlePoint;

	int32 textureX = _crossHairPack._crossHairCenter->GetSizeX();
	int32 textureY = _crossHairPack._crossHairCenter->GetSizeY();

	middlePoint.X = (screenSize.X / 2.f) - (textureX / 2.f);
	middlePoint.Y = (screenSize.Y / 2.f) - (textureY / 2.f);


	DrawTexture(_crossHairPack._crossHairCenter, middlePoint.X, middlePoint.Y, textureX, textureY, 0.f, 0.f, 1.f, 1.f);
	DrawTexture(_crossHairPack._crossHairLeft, middlePoint.X - _recoil, middlePoint.Y, textureX, textureY, 0.f, 0.f, 1.f, 1.f);
	DrawTexture(_crossHairPack._crossHairRight, middlePoint.X + _recoil, middlePoint.Y, textureX, textureY, 0.f, 0.f, 1.f, 1.f);
	DrawTexture(_crossHairPack._crossHairBottom, middlePoint.X, middlePoint.Y + _recoil, textureX, textureY, 0.f, 0.f, 1.f, 1.f);
	DrawTexture(_crossHairPack._crossHairTop, middlePoint.X, middlePoint.Y - _recoil, textureX, textureY, 0.f, 0.f, 1.f, 1.f);

}

