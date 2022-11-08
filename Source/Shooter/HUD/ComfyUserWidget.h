// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComfyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UComfyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UWidgetAnimation* GetAnimation(FString name);
protected:																				
	bool Initialize()override;
																						
			

private:
		TMap<FString, UWidgetAnimation*> _widgetAnimations;

														
};
