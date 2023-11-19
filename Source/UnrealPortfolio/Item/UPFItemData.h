// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UPFItemData.generated.h"

class UGameplayEffect;
/**
 * 아이템 데이터 기반 클래스
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("UPFItemData"), GetFName());
	}

public:
	// 아이템이 가진 특성을 나타내는 태그들
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ItemTags;
};
