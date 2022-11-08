// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "../HUD/OverHeadWidget.h"
#include "../Weapon/Weapon.h"
#include "Components/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "../HUD/CharacterHUD.h"
#include "BlasterAnimInstance.h"
#include "../GameMode/BlasterGameMode.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "BlasterPlayerState.h"
#include "BlasterController.h"
#include "Components/LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	_springArm->SetupAttachment(GetMesh());
	_springArm->TargetArmLength = 600.f;
	_springArm->bUsePawnControlRotation = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);



	_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	_camera->SetupAttachment(_springArm, USpringArmComponent::SocketName);
	_camera->bUsePawnControlRotation = false;
	_camera->PostProcessSettings.DepthOfFieldFocalDistance = 10000.f;
	_camera->PostProcessSettings.DepthOfFieldFstop = 32.f;


	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedges = true;
	GetCharacterMovement()->BrakingFriction = 0.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 800.f;
	GetCharacterMovement()->MaxAcceleration = 1024.f;

	NetUpdateFrequency = 66.f; 
	MinNetUpdateFrequency = 33.f;

	_overHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));	
	_overHeadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	_overHeadWidget->SetupAttachment(RootComponent);


	_combatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	_combatComponent->SetIsReplicated(true);

	_lagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));
	_lagCompensationComponent->SetIsReplicated(true);

	_timelinePack._timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeLine"));


	_head = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadHitBox"));
	_head->SetupAttachment(GetMesh(), FName("Head"));
	_head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Head"), _head);

	_pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("PelvisHitBox"));
	_pelvis->SetupAttachment(GetMesh(), FName("Pelvis"));
	_pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Pelvis"), _pelvis);


	_spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine02HitBox"));
	_spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	_spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("spine_02"), _spine_02);


	_spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine03HitBox"));
	_spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	_spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("spine_03"), _spine_03);


	_upperArm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftUpperArmHitBox"));
	_upperArm_l->SetupAttachment(GetMesh(), FName("UpperArm_L"));
	_upperArm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("UpperArm_L"), _upperArm_l);

	_upperArm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("RightUpperArmHitBox"));
	_upperArm_r->SetupAttachment(GetMesh(), FName("UpperArm_R"));
	_upperArm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("UpperArm_R"), _upperArm_r);


	_lowerArm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftLowerArmHitBox"));
	_lowerArm_l->SetupAttachment(GetMesh(), FName("LowerArm_L"));
	_lowerArm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("LowerArm_L"), _lowerArm_l);


	_lowerArm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLowerArmHitBox"));
	_lowerArm_r->SetupAttachment(GetMesh(), FName("LowerArm_R"));
	_lowerArm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("LowerArm_R"), _lowerArm_r);


	_hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandHitBox"));
	_hand_l->SetupAttachment(GetMesh(), FName("Hand_L"));
	_hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Hand_L"), _hand_l);


	_hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandHitBox"));
	_hand_r->SetupAttachment(GetMesh(), FName("Hand_R"));
	_hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Hand_R"), _hand_r);


	_thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftThighHitBox"));
	_thigh_l->SetupAttachment(GetMesh(), FName("Thigh_L"));
	_thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Thigh_L"), _thigh_l);


	_thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("RightThighHitBox"));
	_thigh_r->SetupAttachment(GetMesh(), FName("Thigh_R"));
	_thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Thigh_R"), _thigh_r);


	_calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCalfHitBox"));
	_calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	_calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("calf_l"), _calf_l);


	_calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCalfHitBox"));
	_calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	_calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("calf_r"), _calf_r);


	_foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFootHitBox"));
	_foot_l->SetupAttachment(GetMesh(), FName("Foot_L"));
	_foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Foot_L"), _foot_l);


	_foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFootHitBox"));
	_foot_r->SetupAttachment(GetMesh(), FName("Foot_R"));
	_foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_lagCompensationComponent->_hitBoxes.Add(FName("Foot_R"), _foot_r);


} 

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	_hp = _maxHp;

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	_controller = Cast<ABlasterController>(Controller);
	if (_controller)
	{
		_hud = Cast<ACharacterHUD>(_controller->GetHUD());
		if (_hud)
		{
			_hud->UpdateStatusHP();
		}
	}
	
	_animInstance = Cast<UBlasterAnimInstance>(GetMesh()->GetAnimInstance());

	//OnCharacterMovementUpdated.AddDynamic(this, &ThisClass::HandleHighPingMovement);


	//if (_crossHair)
	//{
	//	UUserWidget* widget = CreateWidget<UUserWidget>(GetWorld(), _crossHair);
	//	widget->AddToViewport();
	//}
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (_combatComponent)
	{
		_combatComponent->_character = this;
		_combatComponent->_movementComponent = GetCharacterMovement();
		_combatComponent->_playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (_combatComponent->_playerController)
		{
			_combatComponent->_hud = Cast<ACharacterHUD>(_combatComponent->_playerController->GetHUD());
		}
		if (_lagCompensationComponent)
		{
			_lagCompensationComponent->_controller = Cast<ABlasterController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			_lagCompensationComponent->_character = this;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, TEXT("CombatComponent is not initialized, it can get animation not working"));
	}
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_controller || _controller->HasAuthority())
	{
		return;
	}

	if (_lagCompensationComponent && _lagCompensationComponent->IsHighPing())
	{		
		DeaccelerateLocalVelocity();
		_lagCompensationComponent->EnableMoveWithOutCorrection(true);
	}
	else
		_lagCompensationComponent->EnableMoveWithOutCorrection(false);
	

	HideCloseCamera();
}

void ABlasterCharacter::DeaccelerateLocalVelocity()
{
	if (GetCharacterMovement()->GetLastInputVector().Size() == 0.f && _localVelocity.Size() >= 30.f)
	{
		double  deaccel = GetCharacterMovement()->BrakingDecelerationWalking * GetWorld()->DeltaTimeSeconds;
		_localVelocity.X = _localVelocity.X > 0 ? _localVelocity.X - deaccel : _localVelocity.X + deaccel;
		_localVelocity.Y = _localVelocity.Y > 0 ? _localVelocity.Y - deaccel : _localVelocity.Y + deaccel;
		_localVelocity = ClampVector(_localVelocity, -_localVelocity, _localVelocity);

		if (_localVelocity.Size() < 30.f)
		{
			_localVelocity = FVector::ZeroVector;
		}
	}
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &ThisClass::JumpPressed);
	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Released, this, &ThisClass::StopJumping);

	PlayerInputComponent->BindAction(FName("Equip"), EInputEvent::IE_Pressed, this, &ThisClass::Equip);
	PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Pressed, this, &ThisClass::CrouchPressed);
	
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Pressed, this, &ThisClass::AimOn);
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Released, this, &ThisClass::AimOff);

	PlayerInputComponent->BindAction(FName("Sprint"), EInputEvent::IE_Pressed, this, &ThisClass::SprintOn);
	PlayerInputComponent->BindAction(FName("Sprint"), EInputEvent::IE_Released, this, &ThisClass::SprintOff);

	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Pressed, this, &ThisClass::FirePressed);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Released, this, &ThisClass::FireReleased);

	PlayerInputComponent->BindAction(FName("Reload"), EInputEvent::IE_Pressed, this, &ThisClass::ReloadPressed);


	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ThisClass::MoveRight);

	PlayerInputComponent->BindAxis(FName("Turn"), this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ThisClass::LookUp);


}


void ABlasterCharacter::MoveForward(float value)
{
	if (!Controller || abs(value) < 0.3f)
	{
		return;
	}

	// GetForwardVector() is for direction for an actor not a controller

	const FRotator yawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	FVector direction(FRotationMatrix(yawRot).GetUnitAxis(EAxis::X));

	AddMovementInput(direction, value);
	

	if (_controller && _lagCompensationComponent->IsHighPing())
	{
		_localVelocity += GetWorld()->DeltaRealTimeSeconds * direction * value * GetCharacterMovement()->MaxAcceleration;
		_localVelocity=_localVelocity.GetClampedToMaxSize(GetCharacterMovement()->MaxAcceleration);
	}
	
	if (value < 0)
	{
		SprintOff();
	}
}

void ABlasterCharacter::MoveRight(float value)
{
	if (!Controller || abs(value) < 0.3f)
	{
		return;
	}

	const FRotator yawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
	FVector direction(FRotationMatrix(yawRot).GetUnitAxis(EAxis::Y));


	AddMovementInput(direction, value);
	
	if (_controller && _lagCompensationComponent->IsHighPing())
	{
		_localVelocity += GetWorld()->DeltaRealTimeSeconds * direction * value * GetCharacterMovement()->MaxAcceleration;
		_localVelocity = _localVelocity.GetClampedToMaxSize(GetCharacterMovement()->MaxAcceleration);
	}
	

	if (abs(value) > 0)
	{
		SprintOff();
	}
}

void ABlasterCharacter::Turn(float value)
{
	if (!Controller || value == 0)
	{
		return;
	}
	
	AddControllerYawInput(value);
}

void ABlasterCharacter::LookUp(float value)
{
	if (!Controller || value == 0)
	{
		return;
	}

	FRotator rot = Controller->GetControlRotation();

	float pitch =0;
	if (rot.Pitch > 270)
		pitch = rot.Pitch - 360;
	else if (rot.Pitch <90)
		pitch = rot.Pitch;
	

	if (pitch <= 90 && pitch >= -90)
		AddControllerPitchInput(value);
	else
	{
		if (pitch > 20)
		{
			if (value > 0)
				AddControllerPitchInput(value);
		}
		else
		{
			if (value < 0)
				AddControllerPitchInput(value);
		}
	}


}

void ABlasterCharacter::JumpPressed()
{
	if (GetCharacterMovement()->IsCrouching() || _combatComponent->_isAiming)
	{
		UnCrouch();
		_combatComponent->Aim(false);
	}
	else
	{
		
		Jump();
	}
}

void ABlasterCharacter::CrouchPressed()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else if (_combatComponent->_sprinting)
	{
		SprintOff();
	}
	else if ((GetCharacterMovement()->IsFalling()))
	{
		return;
	}

	Crouch();
}

void ABlasterCharacter::Equip()
{
	if (!_combatComponent)
	{
		return;
	}
	
	_combatComponent->EquipWeapon();
}


void ABlasterCharacter::AimOn()
{
	if (!_combatComponent)
	{
		return;
	}
	if (_combatComponent->_sprinting || GetCharacterMovement()->IsFalling() || !_combatComponent->_equippedWeapon)
	{
		return; 
	}
	_combatComponent->ClearState();
	_combatComponent->Aim(true);

}


void ABlasterCharacter::AimOff()
{
	if (!_combatComponent)
	{
		return;
	}

	_combatComponent->Aim(false);

}

void ABlasterCharacter::SprintOn()
{

	if (!_combatComponent || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	FVector velocity = GetTransform().InverseTransformVector(GetCharacterMovement()->Velocity);
	if (velocity.Size() == 0 || velocity.X < 0 || velocity.Z > 100.f)
	{
		return;
	}

	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}

	if (!_combatComponent->_sprinting)
	{
		_combatComponent->ClearState();
		_combatComponent->Sprint(true);
	}

}

void ABlasterCharacter::SprintOff()
{
	if (_combatComponent &&_combatComponent->_sprinting)
	{
		_combatComponent->Sprint(false);
	}

}

void ABlasterCharacter::FirePressed()
{
	if (_combatComponent )
	{	
		_combatComponent->Fire(true);
	}



}

void ABlasterCharacter::FireReleased()
{
	if (_combatComponent)
	{
		_combatComponent->Fire(false);
	}

}

void ABlasterCharacter::ReloadPressed()
{
	if (_combatComponent)
	{
		_combatComponent->Reload();
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	//core logic
	if (_hp == 0.f)
	{
		return;
	}
	_hp = FMath::Clamp(_hp - Damage, 0, 100);
	
	//for server animation or hud (It's only for listen server, don't need this on Dedicated Server)
	ABlasterCharacter* damagedCharacter = Cast<ABlasterCharacter>(DamagedActor);
	ABlasterPlayerState* damagedPlayerState = Cast<ABlasterPlayerState>(damagedCharacter->GetPlayerState());
	UBlasterAnimInstance* animInstance = damagedCharacter ? Cast<UBlasterAnimInstance>(damagedCharacter->GetMesh()->GetAnimInstance()) : nullptr;
	
	ACharacter* instigator = InstigatedBy->GetCharacter();
	ABlasterCharacter* instigatingCharacter = Cast<ABlasterCharacter>(instigator);
	ABlasterPlayerState* instigatorPlayerState = InstigatedBy->GetPlayerState<ABlasterPlayerState>();

	ABlasterGameMode* gameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(GetWorld()));


	if (animInstance)
	{
		animInstance->PlayHitReactMontage();
	}
	
	if (_hud)
	{
		_hud->UpdateStatusHP();
	}

	// if you can get gameMode, it is on server side.
	if (_hp <= 0.0f)
	{
		if (instigatorPlayerState)
		{
			instigatorPlayerState->AddScore(damagedCharacter, instigatingCharacter, 1.f);
		}

		if (damagedPlayerState)
		{
			damagedPlayerState->AddDeath(damagedCharacter);
		}

		if (damagedCharacter)
		{
			Dissolve(damagedCharacter);
			damagedCharacter->_combatComponent->Drop();
			damagedCharacter->GetCharacterMovement()->DisableMovement();

			if (gameMode)
			{
				gameMode->EliminatePlayer(this, damagedCharacter->Controller);
			}
		}
	}
	
}
void ABlasterCharacter::Dissolve(ABlasterCharacter* damagedCharacter)
{
	if (damagedCharacter)
	{
		damagedCharacter->_dissolveMaterialInst = UMaterialInstanceDynamic::Create(damagedCharacter->_dissolveMaterial, this);
		damagedCharacter->_dissolveMaterialInst->SetScalarParameterValue(FName("DissolveParam"), -0.5f);
		damagedCharacter->GetMesh()->SetMaterial(0, damagedCharacter->_dissolveMaterialInst);
		damagedCharacter->_timelinePack._track.BindDynamic(this, &ThisClass::DissolveMaterial);
		damagedCharacter->_timelinePack.StartTimeline();
		damagedCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		damagedCharacter->SetActorEnableCollision(false);
		damagedCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);


		FTransform spawnTransform =   damagedCharacter->GetActorTransform();
		spawnTransform.SetScale3D({ 0.25f,0.25f,1.f });

		if (_dieParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _dieParticle, spawnTransform);
		}
		if (_dieSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), _dieSound, damagedCharacter->GetActorLocation());
		}
	}

}

void ABlasterCharacter::OnRep_HP()
{
	//for clients


	if (_animInstance)
	{
		_animInstance->PlayHitReactMontage();
	} // hit animation
	

	// only for local client
	if (_hud)
	{
		_hud->UpdateStatusHP();
	}
	
	if (_hp ==0.f)
	{
		//disolve Start
		GetCharacterMovement()->DisableMovement();
		Dissolve(this);

	}
}

void ABlasterCharacter::IgnoreErrorCheckForHighPing(bool set)
{
	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = set;
	GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = set;
}

void ABlasterCharacter::IgnoreErrorCheckForHighPing_Server_Implementation(bool set)
{
	IgnoreErrorCheckForHighPing(set);
}



void ABlasterCharacter::DissolveMaterial(float timeLineResult)
{
	if (_dissolveMaterialInst)
	{
		_dissolveMaterialInst->SetScalarParameterValue(FName("DissolveParam"), timeLineResult);
	}
}


void ABlasterCharacter::HideCloseCamera()
{
	if (!IsLocallyControlled() || ! _combatComponent)
	{
		return;
	}
	AWeapon* weapon = _combatComponent->GetEquippedWeapon();
	if (!weapon)
	{
		return;
	}

	if ((GetActorLocation() - _camera->GetComponentLocation()).Size() < 200.f)
	{
		if (GetMesh()->GetVisibleFlag())
		{
			GetMesh()->SetVisibility(false);
		}
		
		if (weapon->GetMesh()->GetVisibleFlag())
		{
			weapon->GetMesh()->SetVisibility(false);
		}
	}
	else
	{
		if (!GetMesh()->GetVisibleFlag())
		{
			GetMesh()->SetVisibility(true);
		}

		if (!weapon->GetMesh()->GetVisibleFlag())
		{
			weapon->GetMesh()->SetVisibility(true);
		}
	}

}


void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass,_hp);
}