// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Weapon.h"
#include "HitScanWeapon.generated.h"

class UParticleSystme;
class USoundCue;

UCLASS()
class SHOOTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitScanWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire(FHitResult& hitResult, bool highPing)override;

	UFUNCTION(Server, Reliable)
	void OnHit(const FHitResult& hitResult);

	UFUNCTION(NetMulticast, Reliable)
		void OnHitMulti(const FHitResult& hitResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* _impactParticle;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		USoundCue* _impactSound;


	UPROPERTY(EditAnywhere)
		float _damage;
};
