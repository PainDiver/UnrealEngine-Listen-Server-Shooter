// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CombatComponent.h"
#include "../Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterController.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UBlasterAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);

	if (!TryGetCharacterComponents())
	{
		return;
	}


	SetMovementState();

	SetCharacterState();

	AdjustOffset(deltaTime);

	AdjustIKFoot();

	AdjustFABRIK();
	
}

void UBlasterAnimInstance::SetMovementState()
{
	FVector velocity;
	if (_controller && !_controller->HasAuthority() && _controller->IsHighPing())
	{
		velocity = _character->GetLocalVelocity();
	}
	else
	{
		velocity = _character->GetVelocity();
	}
	
	velocity.Z = 0.f;
	_speed = velocity.Size();

	velocity = _character->GetTransform().InverseTransformVector(velocity);
	_forward = velocity.X;
	_right = velocity.Y;
}

void UBlasterAnimInstance::SetCharacterState()
{
	_sprinting = _combatComponent->IsSprinting();
	_inAir = _movementComponent->IsFalling();
	_crouched = _movementComponent->IsCrouching();
	_accelerating = abs(_speed) > 0.f;
	_equipped = _combatComponent->IsEquippedWeapon();
	_aiming = _combatComponent->IsAiming();
	_died = _character->GetHP() >0 ? false : true;
	_reloading = _combatComponent->IsReloading();
}

bool UBlasterAnimInstance::TryGetCharacterComponents()
{
	if (_character)
	{
		_movementComponent = _character->GetCharacterMovement();
		_combatComponent = _character->GetCombatComponent();
		_controller = Cast<ABlasterController>(_character->Controller);
	}
	if (!(_character && _movementComponent && _combatComponent && _controller))
	{
		_character = Cast<ABlasterCharacter>(TryGetPawnOwner());
		if (_character)
		{
			_movementComponent = _character->GetCharacterMovement();
			_combatComponent = _character->GetCombatComponent();
			_isLocal = _character->IsLocallyControlled();
			_controller = Cast<ABlasterController>(_character->Controller);
		}
	}

	return _character && _movementComponent && _combatComponent;
}

void UBlasterAnimInstance::PlayFireMontage()
{
	// character fire animation
	if (_character && _combatComponent && _fireAnimMontage && !Montage_IsPlaying(_fireAnimMontage))
	{
		Montage_Play(_fireAnimMontage);
		FName sectionName = _combatComponent->IsAiming() ? "RifleAim" : "RifleHip";
		Montage_JumpToSection(sectionName);
	}
}

void UBlasterAnimInstance::PlayHitReactMontage()
{
	if (_character && _hitReactAnimMontage)
	{
		Montage_Play(_hitReactAnimMontage);
		FName sectionName = FName("HitReact");
		Montage_JumpToSection(sectionName);
	}

}

void UBlasterAnimInstance::PlayReloadMontage()
{
	if (_character && _combatComponent && _reloadAnimMontage && !Montage_IsPlaying(_reloadAnimMontage))
	{
		Montage_Play(_reloadAnimMontage);
		FName sectionName;
		switch (_combatComponent->GetEquippedWeapon()->GetWeaponType())
		{
		case EWeaponType::EWT_RIFLE:
		{
			sectionName = "Rifle";
			break;
		}
		
		}
		Montage_JumpToSection(sectionName);
	}

}





void UBlasterAnimInstance::AdjustOffset(float deltaTime)
{
	FRotator rot = _character->GetBaseAimRotation();
	_pitch = rot.Pitch > 180 ? rot.Pitch - 360 : rot.Pitch;
	
	if (!_accelerating && !_inAir && !_sprinting)
	{
		if (!_turn)
		{
			FRotator currentAimRotation = FRotator(0, _character->GetBaseAimRotation().Yaw, 0.f);
			FRotator deltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, _startRotation);
			_yaw = deltaRotation.Yaw;
			 
			if (abs(_yaw) > 60 && _aiming || abs(_yaw) > 90)
			{
				_turning = _yaw < TURNINGDIRECTION ? TURNINGLEFT : TURNINGRIGHT;
				_turn = true;
			}
		}
		else
		{
			_yaw = FMath::FInterpTo(_yaw, 0.f, deltaTime, 10.f);
			if (abs(_yaw) < 1) //Turn until delta yaw goes below 1
			{
				_startRotation = FRotator(0, _character->GetBaseAimRotation().Yaw, 0.f);
				_yaw = 0.f;
				_turning = 0.f;
				_turn = false;
			}
		}
	}
	else
	{
		_yaw = 0.f;
		_startRotation = FRotator(0, _character->GetBaseAimRotation().Yaw, 0.f);
		_turning = 0.f;
		_turn = false;
	}
}

void UBlasterAnimInstance::AdjustIKFoot()
{
	if (_inAir)
	{
		_leftFootOffset = 0;
		_rightFootOffset = 0;
		return;
	}

	UWorld* world = GetWorld();
	FHitResult hitLeft;
	FHitResult hitRight;

	FVector root = _character->GetMesh()->GetSocketLocation(FName("root"));
	FVector leftFoot = _character->GetMesh()->GetSocketLocation(FName("foot_l"));
	FVector rightFoot = _character->GetMesh()->GetSocketLocation(FName("foot_r"));

	leftFoot.Z = root.Z;
	rightFoot.Z = root.Z;

	FVector leftGround = FVector(leftFoot.X, leftFoot.Y, root.Z - 60);
	FVector rightGround = FVector(rightFoot.X, rightFoot.Y, root.Z - 60);

	if (world)
	{
		world->LineTraceSingleByChannel(hitLeft, leftFoot, leftGround, ECollisionChannel::ECC_Pawn);
		world->LineTraceSingleByChannel(hitRight, rightFoot, rightGround, ECollisionChannel::ECC_Pawn);

		DrawDebugLine(world, leftFoot, leftGround, FColor::Red);
	}

	_leftFootOffset = hitLeft.Distance >5? 0: -hitLeft.Distance;
	_rightFootOffset = hitRight.Distance >5? 0: -hitRight.Distance;

	_leftFootOffset = -hitLeft.Distance;
	_rightFootOffset = -hitRight.Distance;
}

void UBlasterAnimInstance::AdjustFABRIK()
{
	AWeapon* weapon = _combatComponent->GetEquippedWeapon();
	if (!weapon)
	{
		return;
	}

	_leftHandTransform = weapon->GetMesh()->GetSocketTransform(FName("LeftHandSocket"));
	FVector outPos;
	FRotator outRot;
	_character->GetMesh()->TransformToBoneSpace(FName("Hand_R"), _leftHandTransform.GetLocation(), FRotator::ZeroRotator, outPos, outRot);
	_leftHandTransform.SetLocation(outPos);
	_leftHandTransform.SetRotation(FQuat(outRot));
	
	if (_isLocal)
	{
		FVector rightHandLoc = _character->GetMesh()->GetSocketLocation(FName("Hand_R"));
		FVector2D viewportSize;
		GEngine->GameViewport->GetViewportSize(viewportSize);
		FVector worldPos;
		FVector worldDir;
		UGameplayStatics::DeprojectScreenToWorld(_playerController, viewportSize, worldPos, worldDir);

		_rightHandToTargetRotation = UKismetMathLibrary::FindLookAtRotation(rightHandLoc, -(rightHandLoc + worldDir * 3000.f));
	}

}
