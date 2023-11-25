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
#include "DataAssets/UPFCharacterData.h"
#include "UPFCharacterBase.generated.h"

/**
 * 모든 캐릭터들의 base class
 */

class UUPFEquipmentItemData;

UCLASS(Abstract)
class UNREALPORTFOLIO_API AUPFCharacterBase : public ACharacter,
public IMeleeAttackAnimationInterface,
public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPFCharacterBase(const FObjectInitializer& ObjectInitializer);

	// 스스로 정리한 뒤 파괴시킨다. 반드시 캐릭터를 제거할 때는 이 함수를 사용할 것!!
	virtual void DestroySelf();

// ACharacter overrides
protected:
	virtual void PostInitializeComponents() override;
	virtual bool CanCrouch() const override;

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

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	TObjectPtr<UUPFCharacterData> CharacterData;

// Stat
protected:
	// 어트리뷰트 셋 변수가 PlayerState 또는 캐릭터 클래스에 있어야 ASC가 초기화 시 인식할 수 있기 때문에, 여기에 선언함
	UPROPERTY()
	TObjectPtr<UUPFCharacterStatSet> StatSet;

	virtual FName GetStatGroup() {return FName("Default");}

	

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

// Death
private:
	void OnHPZero(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> DeadMontage;
	
	virtual void OnDeathStart() {}
	
	// 데미지를 받아 캐릭터의 체력이 0이 되었을 때 호출되는 함수. 여기서 반드시 FinishDeath 를 호출할 것
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDeathStartCpp")
	void K2_OnDeathStart();
	
	UFUNCTION(BlueprintCallable)
	virtual void FinishDeath();

// Equipment
public:
	// 장비를 장착한다.
	void EquipItem(UUPFEquipmentItemData* EquipmentItemData);

	// UPROPERTY()
	// TWeakObjectPtr<>
};
