// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UPFAbilityInputMappingData.generated.h"


class UInputAction;

/**
 * InputAction과 GameplayTag 매핑을 위한 구조체
 */
USTRUCT(BlueprintType)
struct FUPFAbilityInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * 어떤 키를 눌러야 어빌리티를 발동시킬지 정리해놓은 데이터 리스트
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAbilityInputMappingData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FUPFAbilityInputAction> AbilityInputActions;
};
