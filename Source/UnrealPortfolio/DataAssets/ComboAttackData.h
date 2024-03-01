// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/UPFGameplayAbility.h"
#include "Engine/DataAsset.h"
#include "ComboAttackData.generated.h"

/**
 * 근접공격 애니메이션 몽타주와 코드를 연결하기 위한 데이터 에셋
 */
UCLASS()
class UNREALPORTFOLIO_API UComboAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Montage)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category=Name)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category=Name)
	float FrameRate;

	UPROPERTY(EditAnywhere, EditFixedSize, Category=ComboData)
	TArray<float> EffectiveFrameCount;
};
