// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnrealPortfolio.h"
#include "Engine/DataAsset.h"
#include "UPFAbilityInputMappingData.generated.h"


class UInputAction;
class UUPFGameplayAbility;

/**
 * 어빌리티를 발동시키기 위한 인풋 및 태그를 묶어놓은 구조체
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
	TSubclassOf<UUPFGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EUPFGameplayAbilityInputBinds> InputID;
};

/**
 * 어떤 키를 눌러야 어빌리티를 발동시킬지 정리해놓은 데이터 리스트
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAbilityInputMappingData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 이 캐릭터가 보유한 어빌리티를 발동시키기 위한 입력 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FUPFAbilityTriggerData> AbilityInputActions;
};
