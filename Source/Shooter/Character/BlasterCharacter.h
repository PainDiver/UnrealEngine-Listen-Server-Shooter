// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Utils/GlobalStruct.h"
#include "BlasterCharacter.generated.h"

class UCombatComponent;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputComponent;
class ACharacterHUD;
class UBlasterAnimInstance;
class UParticleSystemComponent;
class USoundCue;
class ABlasterController;
class ULagCompensationComponent;
class UBoxComponent;


UCLASS()
class SHOOTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;

	void DeaccelerateLocalVelocity();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	void JumpPressed();

	void CrouchPressed();

	void Equip();

	void AimOn();
	void AimOff();

	void SprintOn();
	void SprintOff();

	void FirePressed();
	void FireReleased();

	void ReloadPressed();

	void Dissolve(ABlasterCharacter* damagedCharacter);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	UFUNCTION()
	void OnRep_HP();


public:

	void IgnoreErrorCheckForHighPing(bool set);

	UFUNCTION(Server, Reliable)
		void IgnoreErrorCheckForHighPing_Server(bool set);
	

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCombatComponent* GetCombatComponent() { return _combatComponent; }

	FORCEINLINE ACharacterHUD* GetHUD() { return _hud; }

	FORCEINLINE float GetHP() { return _hp; }

	FORCEINLINE float GetMaxHP() { return _maxHp; }

	FORCEINLINE ABlasterController* GetBlasterController() { return _controller; }

	FORCEINLINE FVector GetLocalVelocity() { return _localVelocity; }

private:

	void HideCloseCamera();

	UFUNCTION()
	void DissolveMaterial(float timeLineResult);

private:



	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* _springArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* _camera;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* _overHeadWidget;

	UPROPERTY(VisibleAnywhere)
	UCombatComponent* _combatComponent;

	UPROPERTY(VisibleAnywhere)
		ULagCompensationComponent* _lagCompensationComponent;

	ABlasterController* _controller;


	ACharacterHUD* _hud;

	UBlasterAnimInstance* _animInstance;


	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* _dissolveMaterialInst;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* _dissolveMaterial;

	UPROPERTY(EditAnywhere)
	FTimeLinePack _timelinePack;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* _dieParticle;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		USoundCue* _dieSound;

	//UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	//	TSubclassOf<UUserWidget> _crossHair;


	UPROPERTY(replicatedUsing = OnRep_HP)
	float _hp;

	UPROPERTY(EditAnywhere)
	float _maxHp = 100.f;

	UPROPERTY()
	FVector _localVelocity = FVector::ZeroVector;



///////////////////////////////////////////// hit boxes
	UPROPERTY(EditAnywhere)
		UBoxComponent* _head;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _pelvis;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _spine_02;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _spine_03;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _upperArm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _upperArm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _lowerArm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _lowerArm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _hand_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _hand_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _thigh_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _thigh_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _calf_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _calf_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _foot_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* _foot_r;

};
