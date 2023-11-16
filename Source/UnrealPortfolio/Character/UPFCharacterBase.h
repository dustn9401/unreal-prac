// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "..\Interface\MeleeAttackAnimationInterface.h"
#include "AbilitySystemInterface.h"
#include "Ability/UPFGameplayAbility.h"
#include "Ability/Attributes/UPFCharacterStatSet.h"
#include "Components/UPFAbilitySystemComponent.h"
#include "Components/UPFCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
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
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UUPFAbilitySystemComponent> AbilitySystemComponent;		// 이 캐릭터가 보유한 어빌리티를 관리할 컴포넌트

// Stat
protected:
	// 어트리뷰트 셋 변수가 PlayerState 또는 캐릭터 클래스에 있어야 ASC가 초기화 시 인식할 수 있기 때문에, 여기에 선언함
	UPROPERTY()
	TObjectPtr<UUPFCharacterStatSet> StatSet;

	

public:
	FORCEINLINE TObjectPtr<UUPFCharacterStatSet> GetStatSet() const
	{
		return StatSet;
	}

	// Widget
protected:
	// HP bar 표시용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UWidgetComponent> HPBarWidgetComp;

// Equipment
	

	// UPROPERTY()
	// TWeakObjectPtr<>
};
