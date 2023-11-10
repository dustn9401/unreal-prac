// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/AttackAnimationInterface.h"
#include "AbilitySystemInterface.h"
#include "Ability/UPFGameplayAbility.h"
#include "Components/UPFAbilitySystemComponent.h"
#include "DataAssets/ComboAttackData.h"
#include "UPFCharacterBase.generated.h"

/**
 * 모든 캐릭터들의 base class
 */

UCLASS()
class UNREALPORTFOLIO_API AUPFCharacterBase : public ACharacter,
public IMeleeAttackAnimationInterface,
public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPFCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void PostInitializeComponents() override;

// IAttackAnimationInterface Impl
public:
	virtual void OnMeleeAttackAnimationHit() override;

// IAbilitySystemInterface Impl
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

// Ability
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UUPFAbilitySystemComponent> AbilitySystemComponent;		// 이 캐릭터가 보유한 어빌리티를 관리할 컴포넌트

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboAttackData> MeleeAttackAbilityData;	// 캐릭터의 기본 근접 공격 어빌리티 데이터

	UPROPERTY()
	FGameplayAbilitySpecHandle MeleeAttackAbilitySpecHandle;
};
