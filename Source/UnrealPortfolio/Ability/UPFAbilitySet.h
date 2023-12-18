// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "EnhancedInputComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "UnrealPortfolio.h"
#include "Engine/DataAsset.h"
#include "UPFAbilitySet.generated.h"


class UAttributeSet;
class UGameplayEffect;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;
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

	FUPFAbilityTriggerData()
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
};

/*
 * Ability 또는 Effect 의 부여 후 생성된 Handle을 임시로 저장하는 데이터
 * UUPFAbilitySet::GiveToAbilityComp() 로 부여한 어빌리티들을 다시 회수해야 하는 경우 사용되며,
 * 런타임 에서만 사용되어야 하는 구조체임
 */
USTRUCT(BlueprintType)
struct FUPFGrantedAbilitySetData
{
	GENERATED_BODY()

public:
	FUPFGrantedAbilitySetData()
	{
	}
	
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* AttributeSet);

	void TakeFromCharacter(const IAbilitySystemInterface* ASCInterface);

	bool IsEmpty() const
	{
		return AbilityHandles.Num() == 0 && EffectHandles.Num() == 0 && AttributePtrs.Num() == 0;
	}

private:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> AttributePtrs;
};

/**
 * 캐릭터나 장비 등이 하나씩 가질 수 있는 어빌리티 세트,
 * 캐릭터 기능을 편하게 부여/제거하기 위한 용도로 사용
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 부여할 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FUPFAbilityTriggerData> Abilities;

	// 부여할 Effect 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	// 부여할 스텟 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UAttributeSet>> Attributes;

	/*
	 * ASC에 Ability Set 을 부여한다.
	 * 추후 제거를 원할 경우, OutGrantData 를 저장해 놨다가 사용할것
	 * 어빌리티의 인풋은 여기서 바인딩 하지 않음
	 */
	void GiveToCharacter(const IAbilitySystemInterface* ASCInterface, UObject* SrcObj, FUPFGrantedAbilitySetData* OutGrantData = nullptr) const;
};
