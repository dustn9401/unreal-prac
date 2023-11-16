// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterBase.h"

#include "AbilitySystemGlobals.h"
#include "GameplayAbilitiesModule.h"
#include "UPFGameplayTags.h"
#include "Ability/UPFGameplayAbility_MeleeAttack.h"
#include "Components/CapsuleComponent.h"
#include "Components/UPFAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Physics/UPFCollision.h"
#include "Player/UPFPlayerState.h"
#include "UI/UPFHPBarWidget.h"

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

	// Ability
	AbilitySystemComponent = CreateDefaultSubobject<UUPFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Character Stat
	StatSet = CreateDefaultSubobject<UUPFCharacterStatSet>(TEXT("StatSet"));

	// Widget: HP Bar
	HPBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	HPBarWidgetComp->SetupAttachment(GetMesh(), FName(TEXT("rootSocket")));
	HPBarWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 190.0f));
	static ConstructorHelpers::FClassFinder<UUPFHPBarWidget> HPBarWidgetRef(TEXT("/Game/UnrealPortfolio/UI/WBP_UPFHPBar.WBP_UPFHPBar_C"));
	if (HPBarWidgetRef.Class)
	{
		HPBarWidgetComp->SetWidgetClass(HPBarWidgetRef.Class);
		HPBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		HPBarWidgetComp->SetDrawSize(FVector2D(150.0f, 15.0f));
		HPBarWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AUPFCharacterBase::DestroySelf()
{
}

void AUPFCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 스텟 초기화
	const FName StatGroup(TEXT("Player"));
	IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()->GetAttributeSetInitter()->InitAttributeSetDefaults(AbilitySystemComponent, StatGroup, 1, true);
	StatSet->OnInit();
	StatSet->OnHPZero.AddUObject(this, &AUPFCharacterBase::OnHPZero);

	// 위젯 초기화
	HPBarWidgetComp->InitWidget(); // 여기선 아직 Widget이 생성 안된 상태라 수동으로 초기화
	UUPFHPBarWidget* HPBarWidget = CastChecked<UUPFHPBarWidget>(HPBarWidgetComp->GetWidget());
	HPBarWidget->SetData(StatSet);
}

void AUPFCharacterBase::OnMeleeAttackAnimationHit()
{
	// 로컬 컨트롤러만 Notify를 수신한다.
	if (!IsLocallyControlled()) return;

	const auto AnimatingMeleeAttackAbility = Cast<UUPFGameplayAbility_MeleeAttack>(AbilitySystemComponent->GetAnimatingAbility());
	if (!AnimatingMeleeAttackAbility) return;

	AnimatingMeleeAttackAbility->OnAnimNotify();
}

UAbilitySystemComponent* AUPFCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AUPFCharacterBase::OnHPZero(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetActorEnableCollision(false);	// 시체가 걸리적거리지 않도록
	HPBarWidgetComp->SetHiddenInGame(true);	// 사망 연출 중 hp bar는 숨긴다.
	
	OnDeathStart();
	K2_OnDeathStart();	// 사망 몽타주 재생 등 연출은 여기서 처리
}

void AUPFCharacterBase::FinishDeath()
{
	if (IsFinishDeathInvoked) return;
	IsFinishDeathInvoked = true;

	// todo: 컨트롤러에 알려서 HUD 띄우기
}
