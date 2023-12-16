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
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/UnrealPortfolio/Input/Actions/IA_Jump.IA_Jump'"));
	if (InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionCrouchRef(TEXT("/Script/EnhancedInput.InputAction'/Game/UnrealPortfolio/Input/Actions/IA_Crouch.IA_Crouch'"));
	if (InputActionCrouchRef.Object)
	{
		CrouchAction = InputActionCrouchRef.Object;
	}
}

void AUPFCharacterPlayer::BeginDestroy()
{
	for(auto& Pair : AbilityInputBindingHandles)
	{
		Pair.Value.Empty();
	}

	AbilityInputBindingHandles.Empty();
	
	Super::BeginDestroy();
}

void AUPFCharacterPlayer::BeginPlay()
{
	UPF_LOG(LogTemp, Log, TEXT("Name=%s"), *GetName());
	Super::BeginPlay();

	if (CharacterControlData)
	{
		ApplyCharacterControlData(CharacterControlData);
	}

	if (!InputEnabled())
	{
		EnableInput(CastChecked<APlayerController>(GetController()));
	}
}

void AUPFCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 로컬 컨트롤러 일때만 호출되는 함수
	
	UPF_LOG(LogTemp, Log, TEXT("Name=%s"), *GetName());
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Native Inputs
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUPFCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUPFCharacterPlayer::Move);
	EnhancedInputComponent->BindAction<ACharacter, bool>(CrouchAction, ETriggerEvent::Triggered, this, &ACharacter::Crouch, false);
	EnhancedInputComponent->BindAction<ACharacter, bool>(CrouchAction, ETriggerEvent::Completed, this, &ACharacter::UnCrouch, false);

	// Ability Inputs, 서버에 어빌리티 부여 요청을 보낸다.
	ServerRPCGiveCharacterAbilitySet();
}

void AUPFCharacterPlayer::ApplyCharacterControlData(const UUPFCharacterControlData* Data)
{
	if (!IsLocallyControlled()) return;
	
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

void AUPFCharacterPlayer::ServerRPCGiveCharacterAbilitySet_Implementation()
{
	if (!ensure(HasAuthority())) return;
	
	check(CharacterData);
	check(CharacterData->AbilityInputMappingData);

	// 아래의 함수에서 ClientRPCBindAbilitySetInput 호출되어 클라이언트에 인풋 바인딩
	CharacterData->AbilityInputMappingData->GiveToCharacter(this, this);
}

void AUPFCharacterPlayer::ClientRPCBindAbilitySetInput_Implementation(const UUPFAbilitySet* AbilitySet, int32 GrantKey)
{
	if (!ensure(IsLocallyControlled())) return;
	check(AbilitySet);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensure(EIC)) return;

	TArray<int32> BindingHandles;
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
		BindingHandles.Add(PressedBinding.GetHandle());

		FEnhancedInputActionEventBinding& HeldBinding = EIC->BindAction<UUPFAbilitySystemComponent, FGameplayTag>(
			TriggerData.InputAction,
			ETriggerEvent::Ongoing,
			AbilitySystemComponent,
			&UUPFAbilitySystemComponent::AbilityInputTagPressing,
			TriggerData.InputTag);
		BindingHandles.Add(HeldBinding.GetHandle());
		
		FEnhancedInputActionEventBinding& ReleasedBinding = EIC->BindAction<UUPFAbilitySystemComponent, FGameplayTag>(
			TriggerData.InputAction,
			ETriggerEvent::Completed,
			AbilitySystemComponent,
			&UUPFAbilitySystemComponent::AbilityInputTagReleased,
			TriggerData.InputTag);
		BindingHandles.Add(ReleasedBinding.GetHandle());
	}

	if (BindingHandles.Num() > 0)
	{
		AbilityInputBindingHandles.Emplace(GrantKey, BindingHandles);
	}
}

void AUPFCharacterPlayer::ClientRPCRemoveAbilitySetBind_Implementation(int32 GrantKey)
{
	if (!ensure(IsLocallyControlled())) return;

	TArray<int32>* HandlesPtr = AbilityInputBindingHandles.Find(GrantKey);
	if (HandlesPtr == nullptr) return;

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent);

	for(const int32 Handle : *HandlesPtr)
	{
		EIC->RemoveBindingByHandle(Handle);
	}

	AbilityInputBindingHandles.Remove(GrantKey);
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
