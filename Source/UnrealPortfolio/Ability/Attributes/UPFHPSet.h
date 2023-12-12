// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Ability/Attributes/UPFAttributeSet.h"
#include "UPFHPSet.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, float /*CurrentHP*/, float /*MaxHP*/)

/**
 * 캐릭터가 가지고 있는 스텟 데이터
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFHPSet : public UUPFAttributeSet
{
	GENERATED_BODY()

public:
	UUPFHPSet();

	virtual void OnInit() override;

	ATTRIBUTE_ACCESSORS(UUPFHPSet, CurrentHP);
	ATTRIBUTE_ACCESSORS(UUPFHPSet, MaxHP);

	// for UIs
	mutable FOnHPChangedDelegate OnMaxHPChanged;
	mutable FUPFAttributeEvent OnHPZero;

	// for Damage / Heal Effects
	mutable FUPFAttributeEvent OnTakeDamage;
	mutable FUPFAttributeEvent OnHealing;

private:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHP, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentHP;	// 현재 체력

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHP, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHP;	// 최대 체력

	bool bIsOnHPZeroInvoked;
	float PrevHP = 0.0f;
	float PrevMaxHP = 0.0f;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
	UFUNCTION()
	void OnRep_CurrentHP(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHP(const FGameplayAttributeData& OldValue);
};
