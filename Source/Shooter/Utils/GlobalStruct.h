// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GlobalStruct.generated.h"
/**
 * 
 */

#define QuickDebug_F(arg) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("%f"),arg));
#define QuickDebug_S(arg) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("%s"),*arg));
#define QuickDebug_I(arg) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("%d"),arg));
#define QuickDebug_V(arg) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("%f,%f,%f"), arg.X,arg.Y,arg.Z));

USTRUCT()
struct FCrossHairPack
{
	GENERATED_BODY()
		FCrossHairPack() = default;

	FCrossHairPack(UTexture2D* center, UTexture2D* left, UTexture2D* right, UTexture2D* top, UTexture2D* bottom)
	{
		_crossHairCenter = center;
		_crossHairLeft = left;
		_crossHairRight = right;
		_crossHairTop = top;
		_crossHairBottom = bottom;
	}

	UPROPERTY(EditAnywhere)
		UTexture2D* _crossHairCenter;

	UPROPERTY(EditAnywhere)
		UTexture2D* _crossHairLeft;

	UPROPERTY(EditAnywhere)
		UTexture2D* _crossHairRight;

	UPROPERTY(EditAnywhere)
		UTexture2D* _crossHairTop;

	UPROPERTY(EditAnywhere)
		UTexture2D* _crossHairBottom;

	void SetCrossHairs(const FCrossHairPack& pack)
	{
		_crossHairCenter = pack._crossHairCenter;
		_crossHairLeft = pack._crossHairLeft;
		_crossHairRight = pack._crossHairRight;
		_crossHairTop = pack._crossHairTop;
		_crossHairBottom = pack._crossHairBottom;
	}


	bool IsNull()
	{
		return !(_crossHairCenter || _crossHairLeft || _crossHairRight || _crossHairTop || _crossHairBottom);
	}
};


USTRUCT()
struct FTimeLinePack
{
	GENERATED_BODY()

	FTimeLinePack() = default;

	void StartTimeline()
	{
		if (_curve)
		{
			_timeline->AddInterpFloat(_curve, _track);
			_timeline->Play();
		}
	}

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* _timeline; // 
	
	FOnTimelineFloat _track;
	
	UPROPERTY(EditAnywhere)
	UCurveFloat* _curve;
};


UENUM()
enum class EWeaponType :uint8
{
	EWT_RIFLE UMETA(DisplayName = "Rifle"),

	EWT_MAX UMETA(DisplayName = "Max")

};
ENUM_RANGE_BY_COUNT(EWeaponType, static_cast<uint8>(EWeaponType::EWT_MAX))



enum class EWeaponDefaultBullet :uint8
{
	RIFLE = 120,

};
const EWeaponDefaultBullet GE_weaponDefaultBullet[] = {EWeaponDefaultBullet::RIFLE};
																	