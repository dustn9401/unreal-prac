// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UPFAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UUPFAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	TObjectPtr<class ACharacter> Owner;	// 이 객체를 소유하고 있는 캐릭터

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;	// 캐릭터의 Movement컴포넌트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	FVector Velocity;	// 캐릭터의 현재 속도

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	float GroundSpeed;	// 캐릭터의 현재 땅에서의 속도

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	uint8 bIsIdle : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	float MovingThreshold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	uint8 bIsJumping : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	float JumpingThreshold;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	uint8 bIsCrouching : 1;
};
