// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterPlayer.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Ability/UPFAbilitySet.h"
#include "Components/UPFCharacterEquipmentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataAssets/UPFCharacterControlData.h"
#include "Item/UPFConsumableItemData.h"
#include "Item/UPFEquipmentItemData.h"

AUPFCharacterPlayer::AUPFCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UUPFCharacterControlData> CharacterControlDataRef(TEXT("/Script/UnrealPortfolio.UPFCharacterControlData'/Game/UnrealPortfolio/Player/DA_CharacterControl.DA_CharacterControl'"));
	if (CharacterControlDataRef.Object)
	{
		CharacterControlData = CharacterControlDataRef.Object;
	}
	
	// =============================== Camera 설정 =======================================
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// =============================== Input 설정 =======================================
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/UnrealPortfolio/Input/Actions/IA_Look.IA_Look'"));
	if (InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/UnrealPortfolio/Input/Actions/IA_Move.IA_Move'"));
	if (InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionCrouchRef(TEXT("/Script/EnhancedInput.InputAction'/Game/UnrealPortfolio/Input/Actions/IA_Crouch.IA_Crouch'"));
	if (InputActionCrouchRef.Object)
	{
		CrouchAction = InputActionCrouchRef.Object;
	}
}

void AUPFCharacterPlayer::BeginPlay()
{
	UPF_LOG(LogTemp, Log, TEXT("Name=%s"), *GetName());
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		ApplyCharacterControlData(CharacterControlData);
		
		// 캐릭터 기본 어빌리티의 인풋 바인딩, 다시 제거할 일 없으므로 리턴값은 폐기한다.
		FGuid Unused = BindAbilitySetInput(CharacterData->CharacterAbilitySet);
		
		if (!InputEnabled())
		{
			EnableInput(CastChecked<APlayerController>(GetController()));
		}
	}
}

void AUPFCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Native Inputs
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUPFCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUPFCharacterPlayer::Move);
	EnhancedInputComponent->BindAction<ACharacter, bool>(CrouchAction, ETriggerEvent::Triggered, this, &ACharacter::Crouch, false);
	EnhancedInputComponent->BindAction<ACharacter, bool>(CrouchAction, ETriggerEvent::Completed, this, &ACharacter::UnCrouch, false);
}

void AUPFCharacterPlayer::ApplyCharacterControlData(const UUPFCharacterControlData* Data)
{
	if (!IsLocallyControlled()) return;
	if (!ensure(Data)) return;
	
	// Pawn
	bUseControllerRotationYaw = Data->bUseControllerRotationYaw;

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = Data->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = Data->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = Data->RotationRate;

	// Camera
	CameraBoom->TargetArmLength = Data->TargetArmLength;
	CameraBoom->SocketOffset = Data->SocketOffset;
	CameraBoom->SetRelativeRotation(Data->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = Data->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = Data->bInheritPitch;
	CameraBoom->bInheritYaw = Data->bInheritYaw;
	CameraBoom->bInheritRoll = Data->bInheritRoll;
	CameraBoom->bDoCollisionTest = Data->bDoCollisionTest;

	// PlayerController 에게 InputMappingContext 추가하는부분
	// 아래 코드 없으면 인풋이 전달 안됨
	const APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	check(Subsystem);
	check(Data->InputMappingContext);
	
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(Data->InputMappingContext, 0);
}

FGuid AUPFCharacterPlayer::BindAbilitySetInput(const UUPFAbilitySet* AbilitySet)
{
	if (!ensure(IsLocallyControlled())) return FGuid();
	check(AbilitySet);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensure(EIC)) return FGuid();
	
	const FGuid GrantKey = FGuid::NewGuid();
	for(const FUPFAbilityTriggerData& TriggerData : AbilitySet->Abilities)
	{
		if (!IsValid(TriggerData.InputAction)) continue;
		if (!TriggerData.InputTag.IsValid()) continue;
		
		FEnhancedInputActionEventBinding& PressedBinding = EIC->BindAction<UUPFAbilitySystemComponent, FGameplayTag>(
			TriggerData.InputAction,
			ETriggerEvent::Triggered,
			AbilitySystemComponent,
			&UUPFAbilitySystemComponent::AbilityInputTagPressed,
			TriggerData.InputTag);
		AbilityInputBindingHandles.Add(GrantKey, PressedBinding.GetHandle());

		FEnhancedInputActionEventBinding& HeldBinding = EIC->BindAction<UUPFAbilitySystemComponent, FGameplayTag>(
			TriggerData.InputAction,
			ETriggerEvent::Ongoing,
			AbilitySystemComponent,
			&UUPFAbilitySystemComponent::AbilityInputTagPressing,
			TriggerData.InputTag);
		AbilityInputBindingHandles.Add(GrantKey, HeldBinding.GetHandle());
		
		FEnhancedInputActionEventBinding& ReleasedBinding = EIC->BindAction<UUPFAbilitySystemComponent, FGameplayTag>(
			TriggerData.InputAction,
			ETriggerEvent::Completed,
			AbilitySystemComponent,
			&UUPFAbilitySystemComponent::AbilityInputTagReleased,
			TriggerData.InputTag);
		AbilityInputBindingHandles.Add(GrantKey, ReleasedBinding.GetHandle());
	}

	return GrantKey;
}

void AUPFCharacterPlayer::RemoveAbilitySetBind(FGuid GrantGuid)
{
	if (!ensure(IsLocallyControlled())) return;

	TArray<int32> TargetHandles;
	AbilityInputBindingHandles.MultiFind(GrantGuid, TargetHandles);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent);

	for(const int32 Handle : TargetHandles)
	{
		EIC->RemoveBindingByHandle(Handle);
	}

	AbilityInputBindingHandles.Remove(GrantGuid);
}

void AUPFCharacterPlayer::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AUPFCharacterPlayer::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AUPFCharacterPlayer::TakeItem(UUPFItemData* Data)
{
	check(Data);

	if (const UUPFEquipmentItemData* EquipmentData = Cast<UUPFEquipmentItemData>(Data))
	{
		EquipmentComponent->EquipItem(EquipmentData);
	}
	else if (const UUPFConsumableItemData* ConsumableData = Cast<UUPFConsumableItemData>(Data))
	{
		// todo: Consumable 아이템은 픽업 즉시 효과를 적용한다.
	}
	else
	{
		UPF_LOG(LogTemp, Error, TEXT("Invalid Argument Type: %s"), *Data->GetName());
	}
}
