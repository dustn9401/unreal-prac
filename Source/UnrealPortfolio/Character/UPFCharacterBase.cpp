// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterBase.h"

#include "UPFGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/UPFAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Physics/UPFCollision.h"
#include "Player/UPFPlayerState.h"

// Sets default values
AUPFCharacterBase::AUPFCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_UPFCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, 270.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));	// Mesh 대신에 Capsule 사용

	// Skeletal Mesh: 기본 언리얼5 Manny 사용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/UnrealPortfolio/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Animation: 커스텀 클래스 UPFAnimInstance 사용
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/UnrealPortfolio/Animation/ABP_UPFCharacter.ABP_UPFCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UUPFAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AUPFCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AUPFCharacterBase::OnMeleeAttackAnimationHit()
{
	// 본인이 빙의한 캐릭터인 경우에만 서버/클라 분기하여 처리
	if (!IsLocallyControlled()) return;

	
}

UAbilitySystemComponent* AUPFCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
