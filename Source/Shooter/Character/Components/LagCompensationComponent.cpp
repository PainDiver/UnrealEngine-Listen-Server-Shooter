// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "../BlasterController.h"
#include "../BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
ULagCompensationComponent::ULagCompensationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	/*FTimerHandle handle;
	FTimerDelegate dele;
	dele.BindUObject(this, &ThisClass::SaveFrame);
	GetWorld()->GetTimerManager().SetTimer(handle,dele, 0.25, true);*/
}


// Called every frame
void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (_controller &&_controller->IsHighPing())
	//{
	//	SaveFrame();
	//}


	// ...
}

void ULagCompensationComponent::EnableMoveWithOutCorrection(bool on)
{
	if (_character->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection != on)
	{
		_character->IgnoreErrorCheckForHighPing(on);
		_character->IgnoreErrorCheckForHighPing_Server(on);
	}
}

bool ULagCompensationComponent::IsHighPing()
{
	return _controller->IsHighPing();
}

void ULagCompensationComponent::SaveFrame_Implementation()
{
	FHitBoxFramePackage package;
	package._time = GetWorld()->GetTimeSeconds();
	for (auto hitBox : _hitBoxes)
	{
		FHitBoxInfo info;
		info._location = hitBox.Value->GetComponentLocation();
		info._rotation = hitBox.Value->GetComponentRotation();
		info._extent = hitBox.Value->GetScaledBoxExtent();
		package._info.Add(hitBox.Key, info);
	}

	if (!_hitBoxFrames.IsEmpty())
	{
		while (package._time - _hitBoxFrames.GetTail()->GetValue()._time > 5.f)
		{
			_hitBoxFrames.RemoveNode(_hitBoxFrames.GetTail());
		}
	}
	
	_hitBoxFrames.AddHead(package);

	// visual debug
	//for (auto box : _hitBoxFrames.GetHead()->GetValue()._info)
	//{
	//	DrawDebugBox(GetWorld(), box.Value._location, box.Value._extent, FQuat(box.Value._rotation), FColor::Orange, false, 5.f);
	//}
}

void ULagCompensationComponent::CalcHitBoxOnTime_Implementation(float specifiedTime)
{
	_pack._time = 0.f;
	_pack._info.Empty(); 
	
	auto headNode = _hitBoxFrames.GetHead();
	auto tailNode = _hitBoxFrames.GetTail();
	TDoubleLinkedList<FHitBoxFramePackage>::TDoubleLinkedListNode* currentNode =nullptr;

	TDoubleLinkedList<FHitBoxFramePackage>::TDoubleLinkedListNode* youngerNode = nullptr;
	TDoubleLinkedList<FHitBoxFramePackage>::TDoubleLinkedListNode* olderNode = nullptr;

	if (!headNode || !tailNode || headNode->GetValue()._time <specifiedTime || tailNode->GetValue()._time > specifiedTime)
	{
		return;
	}
	
	if ((headNode->GetValue()._time + tailNode->GetValue()._time) / 2.f <= specifiedTime)
	{
		currentNode = headNode;
		while (currentNode->GetValue()._time <= specifiedTime)
		{
			if (currentNode->GetPrevNode())
			{
				currentNode = currentNode->GetPrevNode();
			}
			else
			{
				break;
			}
		}
		olderNode = currentNode;
		youngerNode = currentNode->GetNextNode();
	}
	else
	{
		currentNode = tailNode;
		while (currentNode->GetValue()._time > specifiedTime)
		{
			if (currentNode->GetNextNode())
			{
				currentNode = currentNode->GetNextNode();
			}
			else
			{
				break;
			}
		}
		youngerNode = currentNode;
		olderNode = currentNode->GetPrevNode();
	}
	
	if (!youngerNode || !olderNode)
	{
		return;
	}

	float timeRatio = FMath::Clamp(specifiedTime - olderNode->GetValue()._time / (youngerNode->GetValue()._time - olderNode->GetValue()._time), 0.f, 1.f);
	
	_pack._time = specifiedTime;
	for (auto boxes :_hitBoxes)
	{
		FName boxName = boxes.Key;		
		FHitBoxInfo boxInfo;
		
		boxInfo._location = FMath::VInterpTo(youngerNode->GetValue()._info[boxName]._location, olderNode->GetValue()._info[boxName]._location, 1.f, timeRatio);
		boxInfo._rotation = FMath::RInterpTo(youngerNode->GetValue()._info[boxName]._rotation, olderNode->GetValue()._info[boxName]._rotation, 1.f, timeRatio);
		boxInfo._extent = FMath::VInterpTo(youngerNode->GetValue()._info[boxName]._extent, olderNode->GetValue()._info[boxName]._extent, 1.f, timeRatio);
		
		_pack._info.Add(boxName, boxInfo);
	}
	
	// visual debug

	for (auto box : _pack._info)
	{
		DrawDebugBox(GetWorld(), box.Value._location, box.Value._extent, FQuat(box.Value._rotation), FColor::Orange, false, 5.f);
	}

	return;

}
