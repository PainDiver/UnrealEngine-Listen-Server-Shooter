// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "../../Weapon/Weapon.h"
#include "../BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../../HUD/CharacterHUD.h"
#include "../BlasterAnimInstance.h"
#include "Sound/SoundCue.h"
#include "../BlasterController.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;



	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	InitializeAmmoPacks();

	if (_character&&_character->IsLocallyControlled())
	{
		_defaultFOV = _playerController->PlayerCameraManager->DefaultFOV;
		_currentFOV = _defaultFOV;

		_defaultMaxWalkSpeed = _character->GetCharacterMovement()->MaxWalkSpeed;
	}
}


void UCombatComponent::Sprinting()
{
	if (_sprinting)
	{
		FVector velocity = _character->GetTransform().InverseTransformVector(_movementComponent->Velocity);
		if (velocity.Size() == 0 || velocity.X <0 || velocity.Y> _movementComponent->MaxWalkSpeed / 2.f)
		{
			Sprint(false);
		}
	}
}

void UCombatComponent::SetZoomFOV(float deltaTime)
{
	bool aiming;
	if (_character && _character->GetBlasterController() && _equippedWeapon && _character->GetBlasterController()->IsHighPing())
	{
		aiming = _localAiming;
	}
	else
	{
		aiming = _isAiming;
	}


	_currentFOV = aiming ?
		FMath::FInterpTo(_currentFOV, _equippedWeapon->GetZoomFOV(), deltaTime, _equippedWeapon->GetZoomInterpSpeed())
		: FMath::FInterpTo(_currentFOV, _defaultFOV, deltaTime, _equippedWeapon->GetZoomInterpSpeed());

	_playerController->PlayerCameraManager->SetFOV(_currentFOV);
}

void UCombatComponent::NotifyRecoilToHUD()
{
	if (_hud)
	{
		_hud->SetRecoil(_equippedWeapon->GetRecoil());
	}
	else
	{
		_hud = Cast<ACharacterHUD>(_playerController->GetHUD());
	}
}




// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Sprinting();

	if (_character && _character->IsLocallyControlled() && _equippedWeapon)
	{
		_equippedWeapon->SetRecoilByAction();

		SetZoomFOV(DeltaTime);

		NotifyRecoilToHUD();
	}
}

void UCombatComponent::Aim(bool on)
{
	if (!_equippedWeapon || !_character)
	{
		return;
	}

	if (_character->GetBlasterController() && _character->GetBlasterController()->IsHighPing() && _movementComponent)
	{
		if (!_localAiming && on)
		{
			_localAiming = true;
			_isAiming = true;
			_movementComponent->MaxWalkSpeed = _equippedWeapon->_equippedAimSpeed;
		}
		else
		{
			_localAiming = false;
			_isAiming = false;
			_movementComponent->MaxWalkSpeed = _equippedWeapon->_equippedSpeed;
		}
	}
	Aim_Server(on);
}


void UCombatComponent::Aim_Server_Implementation(bool on)
{
	if (!_equippedWeapon || !_movementComponent)
	{
		return;
	}

	if (!_isAiming && on)
	{
		_isAiming = true;
		_movementComponent->MaxWalkSpeed = _equippedWeapon->_equippedAimSpeed;
	}
	else if(_isAiming )
	{
		_isAiming = false;
		_movementComponent->MaxWalkSpeed = _equippedWeapon->_equippedSpeed;
	}
}

void UCombatComponent::OnRep_Aiming()
{
	// this function is only for handling jittery on 
	if (_character->GetBlasterController() && _character->GetBlasterController()->IsHighPing() && _movementComponent)
	{
		if (_localAiming == false)
		{
			_isAiming = false;
		}
		else
		{
			_isAiming = true;
		}
	}
}

void UCombatComponent::EquipWeapon_Implementation()
{
	if (!_character || !_overlappedWeapon || !_playerController)
	{
		return;
	}

	AWeapon* weaponToEquip = _overlappedWeapon;
	
	//Drop if you already have weapon
	Drop();

	_equippedWeapon = weaponToEquip;
	_equippedWeapon->SetWeaponState(EWeaponState::EWS_EQUIPPED);

	SwapCarriedAmmo(_equippedWeapon->GetWeaponType());

	if (_character->IsLocallyControlled())
	{
		_hud->SetCrossHairTexture(_equippedWeapon->_crossHairPack);
		_hud->UpdateWeaponAmmo(_equippedWeapon->GetAmmo());
		_hud->UpdateWeaponCarriedAmmo(_currentCarriedAmmo);
		_hud->UpdateWeaponName(_equippedWeapon->_displayName);
	}

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), _equippedWeapon->_equipSound,_character->GetActorLocation());

	ClearState();

	const USkeletalMeshSocket* socket = _character->GetMesh()->GetSocketByName("RightHandSocket");
	if (socket)
	{
		socket->AttachActor(_equippedWeapon,_character->GetMesh());
	}

	_equippedWeapon->SetOwner(_character);

	AdjustEquippedMovement();
}


void UCombatComponent::Sprint(bool on)
{
	if (!_character)
	{
		return;
	}

	if (_character->GetBlasterController() && _character->GetBlasterController()->IsHighPing())
	{
		if (!_sprinting && on)
		{
			_sprinting = true;
			_movementComponent->MaxWalkSpeed += 200.f;
		}
		else if (_sprinting)
		{
			_sprinting = false;
			if(_movementComponent->MaxWalkSpeed > _defaultMaxWalkSpeed)
				_movementComponent->MaxWalkSpeed -= 200.f;
		}
	}

	Sprint_Server(on);
}



void UCombatComponent::Sprint_Server_Implementation(bool on)
{
	if (!_sprinting && on)
	{
		_sprinting = true;
		_movementComponent->MaxWalkSpeed += 200.f;
	}
	else if(_sprinting)
	{
		_sprinting = false;
		_movementComponent->MaxWalkSpeed -= 200.f;
	}
		
}


void UCombatComponent::SetOverlappingWeapon(AWeapon* weapon)
{
	if (_character->IsLocallyControlled()) //For authority of self
	{
		if (!weapon)
		{
			if (_overlappedWeapon)
			{
				_overlappedWeapon->ShowPickUpWidget(false);
			}
			_overlappedWeapon = weapon;
		}
		else
		{
			_overlappedWeapon = weapon;
			if (_overlappedWeapon)
			{
				_overlappedWeapon->ShowPickUpWidget(true);
			}
		}
	}
	_overlappedWeapon = weapon;
}

void UCombatComponent::OnRep_OverlapWeapon(AWeapon* weapon)
{
	if (_overlappedWeapon)
	{
		_overlappedWeapon->ShowPickUpWidget(true);
	}
	if (weapon) // weapon value before replication
	{
		weapon->ShowPickUpWidget(false);
	}
}

void UCombatComponent::SwapCarriedAmmo(EWeaponType weaponType)
{
	switch (weaponType)
	{
	case EWeaponType::EWT_RIFLE:
	{
		if (_carriedAmmoPackages.Contains(weaponType))
		{
			_currentCarriedAmmo = _carriedAmmoPackages[weaponType];
		}
		break;
	}

	}
}




void UCombatComponent::OnRep_EquipWeapon()
{
	if (!_character || !_playerController)
	{
		return;
	}

	if(GetWorld() && _equippedWeapon && _character)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _equippedWeapon->_equipSound, _character->GetActorLocation());

	AdjustEquippedMovement();
	
	if (!_character->IsLocallyControlled())
	{
		return;
	}

	
	if (_hud && _equippedWeapon)
	{
		_hud->SetCrossHairTexture(_equippedWeapon->_crossHairPack);
		_hud->UpdateWeaponAmmo(_equippedWeapon->GetAmmo());
		_hud->UpdateWeaponCarriedAmmo(_currentCarriedAmmo);
		_hud->UpdateWeaponName(_equippedWeapon->_displayName);
	}
	else
	{
		_hud->SetCrossHairTexture(FCrossHairPack{});
		_hud->UpdateWeaponAmmo(0);
		_hud->UpdateWeaponCarriedAmmo(0);
		_hud->UpdateWeaponName(FString());
	}

}

void UCombatComponent::OnRep_CarriedAmmo()
{
	if (_equippedWeapon && _character->IsLocallyControlled())
	{

		_hud->UpdateWeaponAmmo(_equippedWeapon->GetAmmo());
		_hud->UpdateWeaponCarriedAmmo(_currentCarriedAmmo);
	}
}

void UCombatComponent::SetFireState_Server_Implementation(bool isFiring)
{
	_firing = isFiring;
}

void UCombatComponent::Fire(bool on)
{
	if (! _equippedWeapon || !_character|| !_character->GetBlasterController() || !_character->IsLocallyControlled() || _reloading || _sprinting )
	{
		return;
	}

	if (on && !_sprinting)
	{
		if (_equippedWeapon->GetAuto())
		{
			GetWorld()->GetTimerManager().SetTimer(_fireTimer, FTimerDelegate::CreateLambda(
			[&]() {
					if (_equippedWeapon->CanFire())
					{
						FHitResult hitResult;	if (_character->GetBlasterController()->IsHighPing())
						{
							SetFireState(false);
						}
						_equippedWeapon->Fire(hitResult, _character->GetBlasterController()->IsHighPing());
					}
					else
					{
						StopFire();
					}
				}
			), _equippedWeapon->_fireRate, true, 0.f);
		}
		else
		{
			FHitResult hitResult;
			_equippedWeapon->Fire(hitResult, _character->GetBlasterController()->IsHighPing());
		}
		SetFireState(true);
	}
	else
	{
		StopFire();
	}

}

void UCombatComponent::StopFire()
{
	if (_character->GetBlasterController()->IsHighPing())
	{
		SetFireState(false);
	}
	SetFireState_Server(false);

	if (_equippedWeapon->GetAuto())
	{
		GetWorld()->GetTimerManager().ClearTimer(_fireTimer);
	}
}

void UCombatComponent::SetFireState(bool isFiring)
{
	if (_character->GetBlasterController()->IsHighPing())
	{
		_firing = isFiring;
	}
	SetFireState_Server(true);
}

void UCombatComponent::Reload()
{
	if (_reloading || _firing)
	{
		return;
	}

	if (_character->GetBlasterController()->IsHighPing())
	{
		Aim(false);
		_reloading = true;
		UBlasterAnimInstance* animInstance = Cast<UBlasterAnimInstance>(_character->GetMesh()->GetAnimInstance());
		if (animInstance)
		{
			animInstance->PlayReloadMontage();
		}
	}

	Reload_Server();
}

void UCombatComponent::Reload_Server_Implementation()
{
	//animation -> notify Reload(server)

	if (_reloading ||_firing)
	{
		return;
	}
	Aim(false);
	_reloading = true;
	Reload_Multi();
}

void UCombatComponent::Reload_Multi_Implementation()
{
	if (_character && _character->GetBlasterController() && _character->GetBlasterController()->IsHighPing())
	{
		return;
	}

	if (_character)
	{
		UBlasterAnimInstance* animInstance = Cast<UBlasterAnimInstance>(_character->GetMesh()->GetAnimInstance());
		if (animInstance)
		{
			animInstance->PlayReloadMontage();
		}
	}
}

void UCombatComponent::ReloadRounds()
{
	if (_equippedWeapon && _currentCarriedAmmo != 0)
	{
		uint32 capacityLeft = _equippedWeapon->_maxAmmo - _equippedWeapon->_currentAmmo;
		uint32 RoundsToReload = FMath::Clamp(_currentCarriedAmmo, static_cast<uint32>(0), capacityLeft);
		_currentCarriedAmmo -= RoundsToReload;
		_equippedWeapon->_currentAmmo += RoundsToReload;

		if (_carriedAmmoPackages.Contains(_equippedWeapon->GetWeaponType()))
		{
			_carriedAmmoPackages[_equippedWeapon->GetWeaponType()] = _currentCarriedAmmo;
		}

		if (_character->IsLocallyControlled())
		{
			_hud->UpdateWeaponAmmo(_equippedWeapon->GetAmmo());
			_hud->UpdateWeaponCarriedAmmo(_currentCarriedAmmo);
		}
		_reloading = false;
	}
}

void UCombatComponent::Drop()
{
	if (_equippedWeapon)
	{		
		if (_firing)
		{
			SetFireState(false);
			GetWorld()->GetTimerManager().ClearTimer(_fireTimer);
		}
		_equippedWeapon->Dropped();
		_equippedWeapon->SetOwner(nullptr);
		_equippedWeapon = nullptr;
	}
}

void UCombatComponent::ClearState()
{
	Sprint(false);
	Aim(false);
	
}

void UCombatComponent::AdjustEquippedMovement()
{
	if (_movementComponent && _character && _equippedWeapon)
	{
		_movementComponent->MaxWalkSpeed = _equippedWeapon->_equippedSpeed;
		_character->bUseControllerRotationYaw = true;
		_movementComponent->bOrientRotationToMovement = false;
	}
}

void UCombatComponent::InitializeAmmoPacks()
{
	int i = 0;
	for (auto Iter : TEnumRange<EWeaponType>())
	{
		_carriedAmmoPackages.Emplace(Iter,static_cast<uint8>(GE_weaponDefaultBullet[i]));
		i++;
	}

}


void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, _overlappedWeapon,COND_OwnerOnly);
	DOREPLIFETIME(ThisClass, _equippedWeapon);
	DOREPLIFETIME(ThisClass, _isAiming);
	DOREPLIFETIME(ThisClass, _sprinting);
	DOREPLIFETIME(ThisClass, _firing);
	DOREPLIFETIME(ThisClass, _reloading);

	DOREPLIFETIME_CONDITION(ThisClass, _currentCarriedAmmo, COND_OwnerOnly);
}
