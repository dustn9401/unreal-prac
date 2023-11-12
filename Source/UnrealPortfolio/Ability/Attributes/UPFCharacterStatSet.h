// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Ability/Attributes/UPFAttributeSet.h"
#include "UPFCharacterStatSet.generated.h"

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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
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
};
