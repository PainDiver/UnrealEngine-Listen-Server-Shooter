// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Weapon.h"
#include "ProjectileWeapon.generated.h"


class AProjectile;
class AWeaponShell;
UCLASS()
class SHOOTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileWeapon();

	UFUNCTION(Server, Reliable)
		void OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit, float damage);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire(FHitResult& hitResult, bool highPing)override;

	void CreateProjectile(const FVector& projectileDir, const FTransform& muzzleTransform,bool highPing,bool skipHit);

	UFUNCTION(Server, Reliable)
		void CreateProjectile_Server(const FVector& projectileDir, const FTransform& muzzleTransform,bool highPing);


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;




public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> _projectileClass;

	UPROPERTY(EditAnywhere)
	float _projectileSpeed;


};
