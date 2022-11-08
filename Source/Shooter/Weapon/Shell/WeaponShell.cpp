// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponShell.h"

// Sets default values
AWeaponShell::AWeaponShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMesh"));
	SetRootComponent(_mesh);

	_mesh->SetSimulatePhysics(true);	
	_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	_mesh->SetNotifyRigidBodyCollision(true);

}

// Called when the game starts or when spawned
void AWeaponShell::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3.f);
	//_mesh->OnComponentHit.AddDynamic(this,&ThisClass::OnMeshHit);
	_mesh->AddImpulse(GetActorRightVector() * 5.f);
}

// Called every frame
void AWeaponShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponShell::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

