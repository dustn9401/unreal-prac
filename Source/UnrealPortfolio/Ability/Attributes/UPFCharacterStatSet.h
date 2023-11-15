// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Ability/Attributes/UPFAttributeSet.h"
#include "UPFCharacterStatSet.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, float /*CurrentHP*/, float /*MaxHP*/)

/**
 * 캐릭터가 가지고 있는 스텟 데이터
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFCharacterStatSet : public UUPFAttributeSet
{
	GENERATED_BODY()

public:
	UUPFCharacterStatSet();

	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, CurrentHP);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, MaxHP);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, Attack);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, AttackRange);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, MovementSpeed);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, Defense);

	// for UIs
	FOnHPChangedDelegate OnHPChanged;
	FUPFAttributeEvent OnDeath;

	// for Damage / Heal Effects
	FUPFAttributeEvent OnTakeDamage;
	FUPFAttributeEvent OnHealing;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHP, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentHP;	// 현재 체력

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHP, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHP;	// 최대 체력

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attack;	// 공격력

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_AttackRange, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRange;	// 공격 범위

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed;	// 공격 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeed;	// 이동속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = "UPF|Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Defense;	// 방어력

	bool bIsOnDeathInvoked;

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

	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue);

public:
	FString ToString() const;
};
