// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class UBoxComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class USoundCue;
class AWeapon;
class AProjectileWeapon;

UCLASS()
class SHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()

	friend AProjectileWeapon;

public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Initialize();

	UFUNCTION(NetMulticast, UnReliable)
		void EffectOnDestroyed();

	UProjectileMovementComponent* GetProjectileMovement() { return _projectileMovement; }


private:


	bool _skipHit = false;

	bool _highPing = false;

	AProjectileWeapon* _parentWeapon;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* _collisionBox;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* _projectileMovement;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess="true"))
	UParticleSystem* _trail;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* _impactParticle;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		USoundCue* _impactSound;


	UPROPERTY(EditAnywhere)
		float _damage;
};
