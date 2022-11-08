// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComfyUserWidget.h"
#include "../Utils/GlobalStruct.h"
#include "WeaponStatusWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;
UCLASS()
class SHOOTER_API UWeaponStatusWidget : public UComfyUserWidget
{
	GENERATED_BODY()



	friend class ACharacterHUD;

	void SetCarriedAmmo(int ammo);

	void SetCurrentAmmo(int ammo);

	void SetWeaponName(FString name);



private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* _carriedAmmo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* _currentAmmo;
	
	UPROPERTY(meta = (BindWidget))
		UTextBlock* _weaponName;

};
