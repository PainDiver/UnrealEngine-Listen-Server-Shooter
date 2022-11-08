// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"

#include "../../Character/BlasterCharacter.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "../Weapon.h"
#include "../../Character/Components/CombatComponent.h"
#include "../../Character/BlasterController.h"
#include "ProjectileWeapon.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	_projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	_collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	_collisionBox->SetIsReplicated(true);

	SetRootComponent(_collisionBox);

	_projectileMovement->SetIsReplicated(true);

	_collisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	_collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	_collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	_collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);



	_projectileMovement->bRotationFollowsVelocity = true;
	_projectileMovement->MaxSpeed = 20000.f;
	_projectileMovement->InitialSpeed = 20000.f;


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	Initialize();

	// destroy the second projectile created by server in owning client.
	// first projectile created in client will do



}

void AProjectile::Destroyed()
{	
	Super::Destroyed();
}



// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectile::Initialize()
{
	UGameplayStatics::SpawnEmitterAttached(_trail, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::Type::KeepWorldPosition);
	SetLifeSpan(4.f);
	_collisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnBoxHit);

}

void AProjectile::OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// this function is bound to collision box only in server.
	
	if(HasAuthority() && !_highPing)
	{
		// this case is ideal situation where there's no lag
		ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
		ACharacter* actor = Cast<ACharacter>(OtherActor);

		ABlasterController* controller = nullptr;
		if (character && actor)
		{
			controller = Cast<ABlasterController>(character->Controller);
			if (controller)
			{
				UGameplayStatics::ApplyDamage(actor, _damage, character->Controller, this, UDamageType::StaticClass());
			}
		}
	}
	else
	{
		if (_highPing && _parentWeapon && !_skipHit)
		{
			// if highPing, it means client projectile has to handle the onBoxHit, but client projectile has no
			// power to execute server RPC, so it depends on parent weapon.
			_parentWeapon->OnBoxHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit, _damage);
			if (_impactParticle)
			{
				UWorld* world = GetWorld();
				FTransform transform = GetActorTransform();
				transform.SetScale3D(FVector(1, 1, 1));
				UGameplayStatics::SpawnEmitterAtLocation(world, _impactParticle, transform);
			}
			if (_impactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, _impactSound, GetActorLocation());
			}
		}
	}


	EffectOnDestroyed();

	Destroy();
}


void AProjectile::EffectOnDestroyed_Implementation()
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
	if (character && character->IsLocallyControlled())
	{
		return;
	}

	if (_impactParticle)
	{
		UWorld* world = GetWorld();
		FTransform transform = GetActorTransform();
		transform.SetScale3D(FVector(1, 1, 1));
		UGameplayStatics::SpawnEmitterAtLocation(world, _impactParticle, transform);
	}
	if (_impactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, _impactSound, GetActorLocation());
	}
}

