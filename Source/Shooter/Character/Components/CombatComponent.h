// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Utils/GlobalStruct.h"
#include "CombatComponent.generated.h"

class UCharacterMovementComponent;
class UWidgetComponent;
class ACharacterHUD;
class AWeapon;
class USoundCue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ABlasterCharacter;

public:	
	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Aim(bool on);

	UFUNCTION(Server, Reliable)
	void Aim_Server(bool on);

	UFUNCTION(Server,Reliable)
	void EquipWeapon();


	void Sprint(bool on);

	UFUNCTION(Server, Reliable)
	void Sprint_Server(bool on);


	void Fire(bool on);

	void StopFire();

	UFUNCTION(Server, Reliable)
		void SetFireState_Server(bool isFiring);

	void SetFireState(bool isFiring);

	void Reload();

	UFUNCTION(Server, Reliable)
	void Reload_Server();

	UFUNCTION(NetMulticast,Reliable)
	void Reload_Multi();

	UFUNCTION(BlueprintCallable)
	void ReloadRounds();


	void Drop();

	void SetOverlappingWeapon(AWeapon* weapon);

	void SwapCarriedAmmo(EWeaponType weaponType);

	UFUNCTION()
	void OnRep_OverlapWeapon(AWeapon* weapon);

	UFUNCTION()
	void OnRep_EquipWeapon();

	UFUNCTION()
	void OnRep_CarriedAmmo();

	
	FORCEINLINE bool IsEquippedWeapon() { return _equippedWeapon ? true : false; }

	FORCEINLINE bool IsAiming() { return _isAiming; }

	FORCEINLINE bool IsSprinting() { return _sprinting; }

	FORCEINLINE bool IsFiring() { return _firing; }

	FORCEINLINE bool IsReloading() { return _reloading; }

	void ClearState();

	void AdjustEquippedMovement();

	void InitializeAmmoPacks();



	FORCEINLINE uint32 GetWeaponAmmo(EWeaponType weaponType) { return _carriedAmmoPackages[weaponType]; }

	FORCEINLINE AWeapon* GetEquippedWeapon() { return _equippedWeapon; }


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;

private:


	void Sprinting();

	void SetZoomFOV(float deltaTime);

	void NotifyRecoilToHUD();

	UFUNCTION()
	void OnRep_Aiming();


private:

	UPROPERTY(ReplicatedUsing = OnRep_OverlapWeapon)
	AWeapon* _overlappedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_EquipWeapon)
	AWeapon* _equippedWeapon;



	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool _isAiming;

	bool _localAiming;

	UPROPERTY(Replicated)
	bool _sprinting;


	UPROPERTY(Replicated)
	bool _firing;

	UPROPERTY(Replicated)
	bool _reloading;


	UPROPERTY(EditAnywhere, Category = "CameraSetting")
		float _defaultFOV;

	float _currentFOV;


	ABlasterCharacter* _character;

	APlayerController* _playerController;

	ACharacterHUD* _hud;

	UCharacterMovementComponent* _movementComponent;

	FTimerHandle _fireTimer;

	UPROPERTY(replicatedUsing= OnRep_CarriedAmmo)
	uint32 _currentCarriedAmmo;

	TMap<EWeaponType, uint32> _carriedAmmoPackages;


	float _defaultMaxWalkSpeed;
};
