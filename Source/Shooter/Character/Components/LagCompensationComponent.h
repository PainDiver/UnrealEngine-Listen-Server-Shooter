// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"


class UBoxComponent;
class ABlasterController;
class ABlasterCharacter;

struct FHitBoxInfo
{
	FVector _location;
	FRotator _rotation;
	FVector _extent;
};

USTRUCT()
struct FHitBoxFramePackage
{
	GENERATED_BODY()

	float _time;

	TMap<FName,FHitBoxInfo> _info;
	
};

/*
This class is for server rewinding but,
if you calculate the result of line trace in Client,
you can avoid using this class.

it's way cheaper to let client execute line trace since this class save frames in tick.
but it can be used for figuring out past client state in server time.

*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ABlasterCharacter;
	 
public:	
	// Sets default values for this component's properties
	ULagCompensationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	UFUNCTION(Server,Reliable)
	void SaveFrame();
	
	void EnableMoveWithOutCorrection(bool on);

	FHitBoxFramePackage GetHitBoxOnTime() { return _pack; }

	bool IsHighPing();


private:

	UFUNCTION(Server, Reliable)
	void CalcHitBoxOnTime(float specifiedTime);

	ABlasterCharacter* _character;

	ABlasterController* _controller;

	TMap<FName, UBoxComponent*> _hitBoxes;
	

	TDoubleLinkedList<FHitBoxFramePackage> _hitBoxFrames;

	FHitBoxFramePackage _pack;

};
