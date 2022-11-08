// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "../Character/BlasterCharacter.h"
#include "../Character/Components/CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Shell/WeaponShell.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/BlasterAnimInstance.h"
#include "../HUD/CharacterHUD.h"
#include "Sound/SoundCue.h"
#include "../Character/BlasterController.h"

// Sets default values
AWeapon::AWeapon()
{
 	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(_mesh);

	_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_mesh->SetSimulatePhysics(true);
	_mesh->SetIsReplicated(true);

	_areaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	_areaSphere->SetupAttachment(RootComponent);
	_areaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

	_pickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	_pickUpWidget->SetWidgetSpace(EWidgetSpace::Screen);
	_pickUpWidget->SetupAttachment(RootComponent);
	_pickUpWidget->SetVisibility(false);


	_equippedSpeed = 500.f;
	_equippedAimSpeed = 200.f;

	bReplicates = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	_currentAmmo = _maxAmmo;

	_areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_areaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	_areaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	_areaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndSphereOverlap);
	

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::Fire(FHitResult& hitResult, bool highPing)
{
	EjectShell();


	if (highPing)
	{
		PlayFireAnimation();
		SpendRound();
	}
	else
	{
		SpendRound_Server();
	}

	PlayFireAnimation_Server(highPing);
	
	ABlasterCharacter* character = GetOwner<ABlasterCharacter>();
	if (character && character->IsLocallyControlled())
	{
		character->GetHUD()->UpdateWeaponAmmo(_currentAmmo);
	}
}

void AWeapon::PlayFireAnimation()
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
	if (character)
	{
		UBlasterAnimInstance* animInstance = Cast<UBlasterAnimInstance>(character->GetMesh()->GetAnimInstance());
		if (animInstance)
		{
			animInstance->PlayFireMontage();
		}
	}
	_mesh->PlayAnimation(_fireAnim, false);
}

void AWeapon::SpendRound()
{
	_currentAmmo = FMath::Clamp(_currentAmmo - 1, static_cast<uint32>(0), _maxAmmo);
	ABlasterCharacter* character = GetOwner<ABlasterCharacter>();
	if (character && character->IsLocallyControlled())
	{
		character->GetHUD()->UpdateWeaponAmmo(_currentAmmo);
	}
}


void AWeapon::SpendRound_Server_Implementation()
{
	SpendRound();
}

void AWeapon::OnRep_SpentRound()
{
	ABlasterCharacter* character = GetOwner<ABlasterCharacter>();
	if (character && character->IsLocallyControlled())
	{
		character->GetHUD()->UpdateWeaponAmmo(_currentAmmo);
	}
}

void AWeapon::PlayFireAnimation_Server_Implementation(bool highPing)
{
	PlayFireAnimation_NetMulti(highPing);	
}


void AWeapon::PlayFireAnimation_NetMulti_Implementation(bool highPing)
{
	// weapon fire animation
	if (highPing)
	{
		ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
		if (character && character->IsLocallyControlled())
		{
			return;
		}
	}
	PlayFireAnimation();
}



void AWeapon::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(otherActor);
	if (character && _pickUpWidget)
	{
		character->GetCombatComponent()->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnEndSphereOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex)
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(OtherActor);
	if (character && _pickUpWidget)
	{
		character->GetCombatComponent()->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch(_weaponState)
	{
		case EWeaponState::EWS_EQUIPPED:
		{
			ShowPickUpWidget(false); //Turn off Widget for client
			GetMesh()->SetSimulatePhysics(false);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		}
		case EWeaponState::EWS_DROPPED:
		{
			ShowPickUpWidget(false);
			GetMesh()->SetSimulatePhysics(true);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		}
	}
}




void AWeapon::SetWeaponState(EWeaponState state)
{
	_weaponState = state;

	switch (_weaponState)
	{
		case EWeaponState::EWS_EQUIPPED:
		{	
			ShowPickUpWidget(false);
			GetMesh()->SetSimulatePhysics(false);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			_areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		}
		case EWeaponState::EWS_DROPPED:
		{
			ShowPickUpWidget(false);
			GetMesh()->SetSimulatePhysics(true);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			_areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		}
	}
}


void AWeapon::ShowPickUpWidget(bool on)
{
	if (_pickUpWidget)
	{
		_pickUpWidget->SetVisibility(on);
	}
}

void AWeapon::EjectShell()
{
	UWorld* world = GetWorld();
	FTransform ejectTransform = GetMesh()->GetSocketTransform(FName("AmmoEject"));
	AWeaponShell* shell = world->SpawnActor<AWeaponShell>(_projectileShellClass, ejectTransform);
}


void AWeapon::SetRecoilByAction()
{
	ABlasterCharacter* character = Cast<ABlasterCharacter>(GetOwner());
	UCombatComponent* combatComponent = nullptr;
	if (character)
	{
		combatComponent = character->GetCombatComponent();
	}

	if (!character || !combatComponent)
	{
		return;
	}

	_currentRecoil = (character->GetCharacterMovement()->Velocity.Size() > 0) ? (_recoil + 8.f) : (_recoil);
	_currentRecoil = (character->GetCharacterMovement()->IsFalling()) ? (_currentRecoil + 30.f) : (_currentRecoil);
	_currentRecoil = (character->GetCharacterMovement()->IsCrouching()) ? (_currentRecoil/2.f) : (_currentRecoil);
	_currentRecoil = combatComponent->IsAiming() ? (_currentRecoil / 5.f) : (_currentRecoil);
}

void AWeapon::CalcRecoilMiddlePoint(FVector2D& middlePoint)
{

	float xRandRecoil = FMath::RandRange(-_currentRecoil, _currentRecoil);
	float yRandRecoil = FMath::RandRange(-_currentRecoil, _currentRecoil);
	middlePoint.X -= xRandRecoil;
	middlePoint.Y -= yRandRecoil;		
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_DROPPED);

	FDetachmentTransformRules rule{ EDetachmentRule::KeepWorld,EDetachmentRule::KeepRelative,EDetachmentRule::KeepWorld,false };
	DetachFromActor(rule);
}




void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, _weaponState);
	DOREPLIFETIME(ThisClass,_currentAmmo);
}