// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

/**
 * 
 */

class UTextBlock;


UCLASS()
class SHOOTER_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()
	


public:

	UFUNCTION(BlueprintCallable)
		void ShowPlayerNetRole(APawn* pawn);

	void SetDisplayText(FString text);
	

protected:
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);


public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* _displayText;



};
