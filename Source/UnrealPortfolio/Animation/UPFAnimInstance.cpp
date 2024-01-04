// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UPFAnimInstance.h"

#include "Character/UPFCharacterBase.h"
#include "Components/UPFCharacterEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UUPFAnimInstance::UUPFAnimInstance(): GroundSpeed(0), bIsMoving(0), MovingThreshold(3.0f), bIsFalling(0), bIsJumping(0), JumpingThreshold(100.0f), bIsCrouching(0), bIsHolstered(0),
                                      AimOffsetYaw(0),
                                      AimOffsetPitch(0),
                                      bIsAiming(0)
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

	if (Owner)
	{
		// 캐릭터가 허리를 얼마나 꺾어야 하는지 계산
		const FRotator& AimRot = Owner->GetBaseAimRotation();
		AimOffsetPitch = FRotator::NormalizeAxis(AimRot.Pitch);
		
		// 캐릭터가 허리를 얼마나 돌려야 하는지 계산
		const FRotator& ActorRot = Owner->GetActorRotation();
		AimOffsetYaw = FRotator::NormalizeAxis(ActorRot.Yaw - AimRot.Yaw);
		
		bIsAiming = Owner->bIsAiming;
		bIsFiring = Owner->bIsFiring;
	}

	if (MovementComponent)
	{
		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsMoving = GroundSpeed > MovingThreshold;
		bIsFalling = MovementComponent->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);
		bIsCrouching = MovementComponent->IsCrouching();

		MovementComponent->MaxWalkSpeed = bIsAiming || bIsFiring ? 230.0f : 500.0f;
	}

	if (EquipmentComponent)
	{
		bIsHolstered = EquipmentComponent->GetIsHolstered();
	}
}
