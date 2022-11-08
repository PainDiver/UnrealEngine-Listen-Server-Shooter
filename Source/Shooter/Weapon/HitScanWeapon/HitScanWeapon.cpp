// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "../../Character/BlasterCharacter.h"
#include "../../Character/BlasterAnimInstance.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "../../Character/BlasterController.h"

// Sets default values
AHitScanWeapon::AHitScanWeapon()
{
 	
}

// Called when the game starts or when spawned
void AHitScanWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHitScanWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHitScanWeapon::Fire(FHitResult& hitResult, bool highPing)
{
	Super::Fire(hitResult,highPing);

	FVector2D viewPortSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(viewPortSize);
	}
	UWorld* world = GetWorld();

	FVector2D middlePoint = { viewPortSize.X / 2.f, viewPortSize.Y / 2.f };
	APlayerController* controller = UGameplayStatics::GetPlayerController(world, 0);
	FVector worldTracePos;
	FVector worldTraceDir;

	CalcRecoilMiddlePoint(OUT middlePoint);

	if (UGameplayStatics::DeprojectScreenToWorld(controller, middlePoint, worldTracePos, worldTraceDir))
	{
		FTransform muzzleTransform = GetMesh()->GetSocketTransform(FName("MuzzleFlash"));
		FVector muzzleLoc = muzzleTransform.GetLocation();
		float adjustDistance = (muzzleLoc - worldTracePos).Size();

		world->LineTraceSingleByChannel(hitResult,
			worldTracePos+ worldTraceDir* adjustDistance,
			worldTracePos + worldTraceDir * 80000.f,
			ECollisionChannel::ECC_Pawn
		);
	}
	
	//hit scan weapon handles linetrace in client and request on hit to server so it can appropriately kill people whatever the ping is

	ABlasterController* blasterController =Cast<ABlasterController>(controller);

	if (hitResult.bBlockingHit)
	{
		if (blasterController->IsHighPing())
		{
			if (_impactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _impactParticle, hitResult.Location, GetActorRotation());
			}
			if (_impactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), _impactSound, hitResult.Location);
			}
		}
		OnHit(hitResult);
	}
}

void AHitScanWeapon::OnHit_Implementation(const FHitResult& hitResult)
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
	ACharacter* actor = Cast<ACharacter>(hitResult.GetActor());

	if (character && actor)
	{
		if (character->Controller)
		{
			UGameplayStatics::ApplyDamage(actor, _damage, character->Controller, this, UDamageType::StaticClass());
		}
	}

	OnHitMulti(hitResult);
}

void AHitScanWeapon::OnHitMulti_Implementation(const FHitResult& hitResult)
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
	if (character->IsLocallyControlled())
	{
		return;
	}

	if (_impactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),_impactParticle, hitResult.Location, GetActorRotation());
	}

	if (_impactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),_impactSound,hitResult.Location);
	}

}
