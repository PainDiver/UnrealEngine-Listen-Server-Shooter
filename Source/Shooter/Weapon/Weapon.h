// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Utils/GlobalStruct.h"
#include "Weapon.generated.h"


class UWidgetComponent;
class USphereComponent;
class UCombatComponent;
class UAnimationAsset;
class AWeaponShell;
class USoundCue;
class ABlasterCharacter;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_INITIAL UMETA(DisplayName = "Initial State"),
	EWS_EQUIPPED UMETA(DisplayName = "Equipped"),
	EWS_DROPPED UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax"),

};

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()

	friend UCombatComponent;
	friend ABlasterCharacter;

public:	
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnSphereOverlap
	(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult
	);

	UFUNCTION()
	virtual void OnEndSphereOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION()
	void OnRep_WeaponState();

	UFUNCTION()
		void OnRep_SpentRound();

	void ShowPickUpWidget(bool on);

	void SetRecoilByAction();

	void CalcRecoilMiddlePoint(FVector2D& middlePoint);

	void Dropped();

	FORCEINLINE void SetAuto(bool aut) { _auto = aut; }

	FORCEINLINE bool GetAuto() { return _auto; }

	FORCEINLINE void SetWeaponState(EWeaponState state);

	FORCEINLINE USphereComponent* GetAreaSphere() { return _areaSphere; }

	FORCEINLINE USkeletalMeshComponent* GetMesh() { return _mesh; }

	FORCEINLINE float GetRecoil() { return _currentRecoil; }

	FORCEINLINE float GetZoomInterpSpeed() { return _zoomInterpSpeed; }

	FORCEINLINE float GetZoomFOV() { return _zoomFOV; }

	FORCEINLINE uint32 GetAmmo() { return _currentAmmo; }

	FORCEINLINE uint32 GetMaxAmmo() { return _maxAmmo; }

	FORCEINLINE EWeaponType GetWeaponType() { return _weaponType; }

	FORCEINLINE bool CanFire() { return _currentAmmo > 0; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

protected:
	virtual void BeginPlay() override;

	virtual void Fire(FHitResult& hitResult, bool highPing);
	
private:

	void EjectShell();


	UFUNCTION(Server, UnReliable)
	void PlayFireAnimation_Server(bool highPing);

	UFUNCTION(NetMulticast,UnReliable)
	void PlayFireAnimation_NetMulti(bool highPing);

	void PlayFireAnimation();


	void SpendRound();

	UFUNCTION(Server,Reliable)
	void SpendRound_Server();

private:

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
		EWeaponState _weaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
		USkeletalMeshComponent* _mesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
		USphereComponent* _areaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
		UWidgetComponent* _pickUpWidget;


	UPROPERTY(EditAnywhere, Category = "Weapon Property")
		UAnimationAsset* _fireAnim; //gun firing


	UPROPERTY(EditAnywhere, Category = "Weapon Property")
		TSubclassOf<AWeaponShell> _projectileShellClass;


	UPROPERTY(EditAnywhere,Category = "Weapon Property")
		float _fireRate = 0.12;


	UPROPERTY(EditAnywhere, Category = "Weapon Property")
		float _equippedSpeed;

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
		float _equippedAimSpeed;

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	float _recoil = 10.f;

	float _currentRecoil;

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	float _zoomFOV = 45.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	float _zoomInterpSpeed = 30.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	bool _auto = true;

	UPROPERTY(EditAnywhere, Category = "Weapon Property")
	FCrossHairPack _crossHairPack;


	UPROPERTY(EditAnywhere)
		USoundCue* _equipSound;

	

	UPROPERTY(replicatedUsing = OnRep_SpentRound)
	uint32 _currentAmmo;

	UPROPERTY(EditAnywhere)
	uint32 _maxAmmo;

	UPROPERTY(EditAnywhere)
	EWeaponType _weaponType;

	UPROPERTY(EditAnywhere)
	FString _displayName;
};
