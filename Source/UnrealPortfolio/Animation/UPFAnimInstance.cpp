// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UPFAnimInstance.h"

#include "Character/UPFCharacterBase.h"
#include "Components/UPFCharacterEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UUPFAnimInstance::UUPFAnimInstance(): GroundSpeed(0), bIsMoving(0), MovingThreshold(3.0f), bIsFalling(0), bIsJumping(0), JumpingThreshold(100.0f), bIsCrouching(0), bIsHolstered(0)
{
}

void UUPFAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<AUPFCharacterBase>(GetOwningActor());
	if (Owner)
	{
		MovementComponent = Owner->GetCharacterMovement();
		EquipmentComponent = Owner->EquipmentComponent;
	}
}

void UUPFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComponent)
	{
		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsMoving = GroundSpeed > MovingThreshold;
		bIsFalling = MovementComponent->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);
		bIsCrouching = MovementComponent->IsCrouching();
	}

	if (EquipmentComponent)
	{
		bIsHolstered = EquipmentComponent->GetIsHolstered();
	}
}
