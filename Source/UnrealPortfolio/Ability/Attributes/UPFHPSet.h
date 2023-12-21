// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Ability/Attributes/UPFAttributeSet.h"
#include "UPFHPSet.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, float /*CurrentHP*/, float /*MaxHP*/)
DECLARE_MULTICAST_DELEGATE(FOnHPZeroDelegate)

/**
 * 캐릭터가 가지고 있는 스텟 데이터
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFHPSet : public UUPFAttributeSet
{
	GENERATED_BODY()

public:
	UUPFHPSet();

	virtual void OnBeginPlay() override;

	ATTRIBUTE_ACCESSORS(UUPFHPSet, CurrentHP);
	ATTRIBUTE_ACCESSORS(UUPFHPSet, MaxHP);

	// Delegates
	mutable FOnHPChangedDelegate OnMaxHPChanged;
	mutable FOnHPChangedDelegate OnCurrentHPChanged;
	mutable FOnHPZeroDelegate OnHPZero;

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
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
	UFUNCTION()
	void OnRep_CurrentHP(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHP(const FGameplayAttributeData& OldValue);
};
