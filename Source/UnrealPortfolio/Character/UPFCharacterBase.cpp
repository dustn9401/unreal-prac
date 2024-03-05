// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterBase.h"

#include "AbilitySystemGlobals.h"
#include "EngineUtils.h"
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
#include "Constants/UPFSocketNames.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Net/UnrealNetwork.h"
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

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		SetData_ServerOnly(CharacterData);	
	}
}

void AUPFCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UPF_LOG(LogTemp, Log, TEXT("Called, %s"), *GetName());

	// 위젯 초기화
	HPBarWidgetComp->InitWidget();
	UUPFHPBarWidget* HPBarWidget = CastChecked<UUPFHPBarWidget>(HPBarWidgetComp->GetWidget());

	UUPFHPSet* HPSet = GetHPSet();
	if (ensure(HPSet))
	{
		HPBarWidget->SetData(HPSet);
		HPSet->OnHPZero.AddUObject(this, &AUPFCharacterBase::OnHPZero);
		HPSet->OnBeginPlay();
	}
}

bool AUPFCharacterBase::CanCrouch() const
{
	return Super::CanCrouch();
}

void AUPFCharacterBase::SetData_ServerOnly(UUPFCharacterData* InData)
{
	if (!ensure(HasAuthority())) return;
	
	check(InData);
	check(InData->CharacterAbilitySet);

	// 아래의 함수에서 어빌리티 및 AttributeSet 을 지급한다.
	InData->CharacterAbilitySet->GiveToAbilityComp(this, this);

	// 지급된 AttributeSet 에 값 입력
	IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()
		->GetAttributeSetInitter()
		->InitAttributeSetDefaults(AbilitySystemComponent, GetStatGroup(), 1, true);
	
	// CurrentHP 값은 테이블에 없기 때문에, 여기서 MaxHP값으로 초기화
	UUPFHPSet* HPSet = GetHPSet();
	if (ensure(HPSet))
	{
		HPSet->SetCurrentHP(HPSet->GetMaxHP());
	}
}

UAbilitySystemComponent* AUPFCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UUPFHPSet* AUPFCharacterBase::GetHPSet() const
{
	for(UAttributeSet* AttrSet :  AbilitySystemComponent->GetSpawnedAttributes())
	{
		if (AttrSet->GetClass() == UUPFHPSet::StaticClass())
		{
			return Cast<UUPFHPSet>(AttrSet);
		}
	}

	return nullptr;
}

void AUPFCharacterBase::OnHPZero()
{
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
	if (HasAuthority())
	{
		bIsAiming = true;
	}
	
	K2_OnAimingStart();

	ModifyReplicationTestProps(true);
	if (HasAuthority())
	{
		MulticastRPCTestFunc(this, CharacterData, UPFGameplayTags::Ability_Active_Holster);
	}
}

void AUPFCharacterBase::OnAimingEnd()
{
	if (HasAuthority())
	{
		bIsAiming = false;
	}
	
	K2_OnAimingEnd();

	ModifyReplicationTestProps(false);
}

void AUPFCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUPFCharacterBase, bIsAiming);
	
	DOREPLIFETIME(AUPFCharacterBase, ReplicatedSubObject);
	DOREPLIFETIME(AUPFCharacterBase, TestStruct);
}

void AUPFCharacterBase::ModifyReplicationTestProps(bool IsStart)
{
	if (!HasAuthority()) return;
	
	if (IsStart)
	{
		if (ReplicateTestSpawnActor)
		{
			// ReplicatedSubObject = GetWorld()->SpawnActor<AActor>(ReplicateTestSpawnActor);
			// ReplicatedSubObject->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, UPFSocketNames::hand_rSocket);
		}

		TestStruct.Integer = 123;
		TestStruct.Integers.Add(1);
		TestStruct.Integers.Add(2);
		TestStruct.Integers.Add(3);
	}
	else
	{
		TestStruct.Integer = 0;
		TestStruct.Integers.Empty();

		if (ReplicatedSubObject)
		{
			ReplicatedSubObject->Destroy();
		}
	}
}

void AUPFCharacterBase::MulticastRPCTestFunc_Implementation(ACharacter* Character, UDataAsset* DataAsset,
	FGameplayTag GameplayTag)
{
}

void AUPFCharacterBase::OnRep_TestStruct()
{
	UPF_LOG(LogTemp, Log, TEXT("Int: %d, Array Num: %d"), TestStruct.Integer, TestStruct.Integers.Num());
}
