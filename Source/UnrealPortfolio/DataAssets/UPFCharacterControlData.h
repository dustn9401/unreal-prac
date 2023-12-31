// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UPFCharacterControlData.generated.h"

/**
 * 플레이어 캐릭터 및 카메라 기본 조작 설정 데이터
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UUPFCharacterControlData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnywhere, Category=Pawn)
	uint32 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category=CharacterMovement)
	uint32 bOrientRotationToMovement : 1;

	UPROPERTY(EditAnywhere, Category=CharacterMovement)
	uint32 bUseControllerDesiredRotation : 1;

	UPROPERTY(EditAnywhere, Category=CharacterMovement)
	FRotator RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=SpringArm)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=SpringArm)
	FVector SocketOffset;

	UPROPERTY(EditAnywhere, Category=SpringArm)
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere, Category=SpringArm)
	uint32 bUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, Category=SpringArm)
	uint32 bInheritPitch : 1;
	
	UPROPERTY(EditAnywhere, Category=SpringArm)
	uint32 bInheritYaw : 1;
    	
	UPROPERTY(EditAnywhere, Category=SpringArm)
	uint32 bInheritRoll : 1;

	UPROPERTY(EditAnywhere, Category=SpringArm)
	uint32 bDoCollisionTest : 1;
};
