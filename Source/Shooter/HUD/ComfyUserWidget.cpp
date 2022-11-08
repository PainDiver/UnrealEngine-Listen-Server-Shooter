// Fill out your copyright notice in the Description page of Project Settings.


#include "ComfyUserWidget.h"
#include "../Utils/GlobalStruct.h"

UWidgetAnimation* UComfyUserWidget::GetAnimation(FString name)
{
	if (_widgetAnimations.Contains(name))
		return _widgetAnimations[name];
	else
		return nullptr;

}

bool UComfyUserWidget::Initialize()
{
	Super::Initialize();
	_widgetAnimations.Empty();
	UWidgetBlueprintGeneratedClass* widgetClass = GetWidgetTreeOwningClass();
	if (!widgetClass)
	{
		return false;
	}
	for (int i = 0; i < widgetClass->Animations.Num(); i++)
	{
		FString name = widgetClass->Animations[i]->GetName();
		_widgetAnimations.Add(name, widgetClass->Animations[i].Get());
	}
	return true;
}
