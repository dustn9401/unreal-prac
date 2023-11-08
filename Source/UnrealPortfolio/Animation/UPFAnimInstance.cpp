// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UPFAnimInstance.h"

UUPFAnimInstance::UUPFAnimInstance(): GroundSpeed(0), bIsIdle(0), MovingThreshold(3.0f), bIsFalling(0), bIsJumping(0), JumpingThreshold(100.0f), TransitionRatio(0.1f)
{
}

void UUPFAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UUPFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}
