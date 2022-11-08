// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComfyUserWidget.h"
#include "../Utils/GlobalStruct.h"
#include "CharacterStatusWidget.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;

UCLASS()
class SHOOTER_API UCharacterStatusWidget : public UComfyUserWidget
{
	GENERATED_BODY()



	friend class ACharacterHUD;



	void SetHPPercent(float hp,float maxHP);

	void SetKill(int kill);

	void SetDeath(int death);

private:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* _hpBar;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* _kill;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* _death;

	UTextBlock* _hpText;
};
