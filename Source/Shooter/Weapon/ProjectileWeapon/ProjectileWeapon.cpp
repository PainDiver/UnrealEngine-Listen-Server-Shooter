// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "../../Character/BlasterCharacter.h"
#include "../../Character/Components/CombatComponent.h"
#include "../Shell/WeaponShell.h"
#include "../../HUD/CharacterHUD.h"
#include "../../Character/BlasterController.h"

// Sets default values
AProjectileWeapon::AProjectileWeapon()
{
 	
}

// Called when the game starts or when spawned
void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectileWeapon::Fire(FHitResult& hitResult, bool highPing)
{
	Super::Fire(hitResult,highPing);

	FVector2D viewPortSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(viewPortSize);
	}
	UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}
	FTransform muzzleTransform = GetMesh()->GetSocketTransform(FName("MuzzleFlash"));
	FVector2D middlePoint = { viewPortSize.X / 2.f, viewPortSize.Y / 2.f };

	APlayerController* controller = UGameplayStatics::GetPlayerController(world, 0);
	if (!controller)
		return;
	
	FVector worldTracePos;
	FVector worldTraceDir;

	CalcRecoilMiddlePoint(OUT middlePoint);

	if (UGameplayStatics::DeprojectScreenToWorld(controller, middlePoint, OUT worldTracePos, OUT worldTraceDir))
	{
		FVector muzzleLoc = muzzleTransform.GetLocation();
		float adjustDistance = (muzzleLoc - worldTracePos).Size();

		world->LineTraceSingleByChannel(hitResult,
			(worldTracePos + adjustDistance * worldTraceDir),
			(worldTracePos + worldTraceDir * 80000.f),
			ECollisionChannel::ECC_WorldDynamic
		); 
		//Linetrace to target from center of the viewport even though projectile comes out of muzzle
	}

	//projectile just follows the direction of muzzle to target if hit
	// or else projectile travels to the position where camera line space ends

	FVector projectileDir;
	projectileDir = hitResult.bBlockingHit ? (hitResult.Location - muzzleTransform.GetLocation()) : ( (worldTracePos + worldTraceDir * 80000.f) - muzzleTransform.GetLocation());
	projectileDir.Normalize();



	// server rewind should be done here.
	// 

	//projectile created in client and onHit function should call server request
	if (highPing)
	{
		CreateProjectile(projectileDir, muzzleTransform, highPing,true);
		CreateProjectile(projectileDir, muzzleTransform, highPing,false);
	}
	else
	{
		CreateProjectile_Server(projectileDir, muzzleTransform, highPing);
	}
}

void AProjectileWeapon::CreateProjectile(const FVector& projectileDir, const FTransform& muzzleTransform,bool highPing,bool skipHit)
{
	FActorSpawnParameters params;
	params.Owner = GetOwner();
	params.Instigator = Cast<APawn>(GetOwner());

	UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}

	AProjectile* projectile = world->SpawnActor<AProjectile>(_projectileClass, muzzleTransform);
	if (projectile)
	{
		projectile->SetOwner(GetOwner());
		projectile->_parentWeapon = this;
		projectile->SetReplicates(true);
		projectile->GetProjectileMovement()->MaxSpeed = _projectileSpeed;
		projectile->GetProjectileMovement()->InitialSpeed = _projectileSpeed;
		projectile->GetProjectileMovement()->Velocity = projectileDir * _projectileSpeed;
		projectile->_highPing = highPing;
		projectile->_skipHit = skipHit;

	}

}


void AProjectileWeapon::CreateProjectile_Server_Implementation(const FVector& projectileDir, const FTransform& muzzleTransform, bool highPing)
{
	CreateProjectile(projectileDir, muzzleTransform, highPing,false);
}


void AProjectileWeapon::OnBoxHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit,float damage)
{
	// this function is bound to collision box only in server.

	ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
	ACharacter* actor = Cast<ACharacter>(OtherActor);

	ABlasterController* controller = nullptr;
	if (character && actor)
	{
		controller = Cast<ABlasterController>(character->Controller);
		if (controller)
		{
			UGameplayStatics::ApplyDamage(actor,damage, character->Controller, this, UDamageType::StaticClass());
		}
	}
}


void AProjectileWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

