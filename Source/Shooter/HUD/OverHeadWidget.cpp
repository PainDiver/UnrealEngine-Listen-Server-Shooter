// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisplayText(FString text)
{
	if (_displayText)
	{
		_displayText->SetText(FText::FromString(text));
	}

}

void UOverHeadWidget::ShowPlayerNetRole(APawn* pawn)
{
	if (!pawn)
	{
		return;
	}
		ENetRole localRole = pawn->GetLocalRole();
	

	FString role;
	switch (localRole)
	{
		case ENetRole::ROLE_Authority:
		{
			role = FString("Authority");
		}
		break;
	
		case ENetRole::ROLE_AutonomousProxy:
		{
			role = FString("AutonomousProxy");
		}
		break;
	
		case ENetRole::ROLE_SimulatedProxy:
		{
			role = FString("SimulatedProxy");
		}
		break;
	
		case ENetRole::ROLE_None:
		{
			role = FString("None");
		}
		break;

	}

	FString localRoleString = FString::Printf(TEXT("Local Role %s"), *role);
	SetDisplayText(localRoleString);

}


void UOverHeadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
