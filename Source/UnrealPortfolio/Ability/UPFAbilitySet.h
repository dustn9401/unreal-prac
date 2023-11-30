// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnrealPortfolio.h"
#include "Engine/DataAsset.h"
#include "UPFAbilitySet.generated.h"


class UGameplayAbility;
class UInputAction;

/**
 * 어빌리티와, 어빌리티를 발동시키기 위한 트리거 정보들을 묶어놓은 구조체
 * 인풋이 필요 없는 패시브어빌리티 일 경우, 인풋 관련 데이터가 없을 수도 있음
 */
USTRUCT(BlueprintType)
struct FUPFAbilityTriggerData
{
	GENERATED_BODY()

	FUPFAbilityTriggerData(): InputID(EUPFGameplayAbilityInputBinds::Ability1)
	{
	}

public:
	// Can be null
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EUPFGameplayAbilityInputBinds> InputID;
};

/**
 * 캐릭터나 장비 등이 하나씩 가질 수 있는 어빌리티 세트
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 이 캐릭터가 보유한 어빌리티를 발동시키기 위한 입력 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FUPFAbilityTriggerData> AbilityInputActions;
};
