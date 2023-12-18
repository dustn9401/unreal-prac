// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterBase.h"

#include "AbilitySystemGlobals.h"
#include "GameplayAbilitiesModule.h"
#include "UnrealPortfolio.h"
#include "UPFGameplayTags.h"
#include "Ability/UPFGameplayAbility_MeleeAttack.h"
#include "Components/CapsuleComponent.h"
#include "Components/UPFAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Ability/UPFAbilitySet.h"
#include "Ability/Attributes/UPFHPSet.h"
#include "Ability/Attributes/UPFStatSet.h"
#include "Components/TimelineComponent.h"
#include "Components/UPFCharacterEquipmentComponent.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/UnrealPortfolio/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Character Stat, 이 포인터 변수들은 클라이언트에서 null이 된다.
	HPSet = CreateDefaultSubobject<UUPFHPSet>(TEXT("HP"));
	StatSet = CreateDefaultSubobject<UUPFStatSet>(TEXT("Stat"));

	// Ability
	AbilitySystemComponent = CreateDefaultSubobject<UUPFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

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

	static ConstructorHelpers::FObjectFinder<UUPFCharacterData> CharacterDataRef(TEXT("/Script/UnrealPortfolio.UPFCharacterData'/Game/UnrealPortfolio/DataAssets/UPFCharacterData.UPFCharacterData'"));
	if (CharacterDataRef.Object)
	{
		CharacterData = CharacterDataRef.Object;
	}

	// Equipment
	EquipmentComponent = CreateDefaultSubobject<UUPFCharacterEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponent->SetIsReplicated(true);

	bIsAiming = false;
}

void AUPFCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 여기서 IsLocallyControlled 가 항상 false임에 주의
	UPF_LOG(LogTemp, Log, TEXT("Called, %s, IsLocallyControlled = %d"), *GetName(), IsLocallyControlled());

	if (HasAuthority())
	{
		SetData_Server(CharacterData);	
	}
}

void AUPFCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UPF_LOG(LogTemp, Log, TEXT("Called, %s"), *GetName());

	// 위젯 초기화
	HPBarWidgetComp->InitWidget();
	UUPFHPBarWidget* HPBarWidget = CastChecked<UUPFHPBarWidget>(HPBarWidgetComp->GetWidget());
	HPBarWidget->SetData(HPSet);
	
	HPSet->OnHPZero.AddUObject(this, &AUPFCharacterBase::OnHPZero);
	HPSet->OnBeginPlay();
}

bool AUPFCharacterBase::CanCrouch() const
{
	return Super::CanCrouch();
}

void AUPFCharacterBase::SetData_Server(UUPFCharacterData* InData)
{
	if (!ensure(HasAuthority())) return;
	
	check(InData);
	check(InData->CharacterAbilitySet);

	// 아래의 함수에서 어빌리티 및 스텟을 초기화 한다.
	InData->CharacterAbilitySet->GiveToCharacter(this, this);

	IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()
		->GetAttributeSetInitter()
		->InitAttributeSetDefaults(AbilitySystemComponent, GetStatGroup(), 1, true);
	
	// CurrentHP 값은 테이블에 없기 때문에, 여기서 MaxHP값으로 초기화
	HPSet->InitCurrentHP(HPSet->GetMaxHP());
}

void AUPFCharacterBase::SetData_Local(UUPFCharacterData* InData)
{
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
	UPF_LOG(LogTemp, Log, TEXT("Called: %s"), *GetName());
	
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetActorEnableCollision(false);	// 시체가 걸리적거리지 않도록
	HPBarWidgetComp->SetHiddenInGame(true);	// 사망 연출 중 hp bar는 숨긴다.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage);
	
	OnDeathStart();
	K2_OnDeathStart();
}

void AUPFCharacterBase::FinishDeath()
{
	Destroy();
}

void AUPFCharacterBase::OnAimingStart()
{
	bIsAiming = true;
	K2_OnAimingStart();
}

void AUPFCharacterBase::OnAimingEnd()
{
	bIsAiming = false;
	K2_OnAimingEnd();
}
