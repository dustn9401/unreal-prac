// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboAttackData.generated.h"

/**
 * AM_Attack 애니메이션 몽타주와 코드를 연결하기 위한 데이터 에셋,
 * AM_Attack 수정 시 반드시 확인할 것
 */
UCLASS()
class UNREALPORTFOLIO_API UComboAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Name)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category=Name)
	float FrameRate;

	UPROPERTY(EditAnywhere, Category=ComboData)
	TArray<float> EffectiveFrameCount;
};
