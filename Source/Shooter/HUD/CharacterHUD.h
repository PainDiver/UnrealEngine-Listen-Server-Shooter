// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "../Utils/GlobalStruct.h"
#include "../HUD/ComfyUserWidget.h"
#include "../HUD/GameStateWidget.h"
#include "CharacterHUD.generated.h"

/**
 * 
 */

class UCharacterStatusWidget;
class UUserWidget;
class APlayerController;
class ABlasterCharacter;
class UWeaponStatusWidget;
class ABlasterGameState;


UENUM()
enum class EWidgetType
{
	EWT_STATUS UMETA(DisplayName = "Status"),
	EWT_WEAPON_STATUS UMETA(DisplayName = "WeaponStatus"),
	EWT_GAMESTATE UMETA(DisplayName = "State"),
	EWT_MAX UMETA(DisplayName = "Max"),
};

UCLASS()
class SHOOTER_API ACharacterHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	
	void SetCrossHairTexture(const FCrossHairPack& pack );

	void BeginPlay()override;
	
	void Tick(float deltaTime)override;

	void SetRecoil(float recoil) { _recoil = recoil; }

	template<typename T>
	T* GetWidget(EWidgetType widgetType) 
	{ 
		if (_widgets.Contains(widgetType))
			return Cast<T>(_widgets[widgetType]);
		else
			return nullptr;
	}

	void UpdateStatusHP();

	void UpdateStatusKill(int kill);

	void UpdateStatusDeath(int death);

	void UpdateWeaponAmmo(int ammo);

	void UpdateWeaponCarriedAmmo(int ammo);

	void UpdateWeaponName(FString name);

	void UpdateTime(float totalTime);

	void UpdateAnnouncement(FString announcement);

	void SetWidgetVisibleState(EWidgetType widgetType, ESlateVisibility setting);

	void SetAllWidgetVisibleState(ESlateVisibility setting);

	template<typename T>
	std::conditional_t<std::is_base_of_v<UComfyUserWidget, T>, void, void> PlayWidgetAnimation(EWidgetType type,FString animationName);

protected:
	virtual void DrawHUD()override;

private:
	void DrawCrossHair();

private:

	TMap<EWidgetType, UUserWidget*> _widgets;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> _statusWidgetClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> _weaponStatusWidgetClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> _gameStateWidgetClass;


	UPROPERTY(EditAnywhere)
	FCrossHairPack _crossHairPack;

	float _recoil;

	APlayerController* _controller;

};


template<typename T>
std::conditional_t<std::is_base_of_v<UComfyUserWidget,T>,void,void> ACharacterHUD::PlayWidgetAnimation(EWidgetType type,FString animationName)
{
	if (_widgets.Contains(type))
	{
		UWidgetAnimation* anim = GetWidget<T>(type)->GetAnimation(animationName+"_INST");
		if (anim)
		{
			GetWidget<T>(type)->PlayAnimation(anim);
		}
	}
}
