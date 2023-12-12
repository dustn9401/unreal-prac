// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Attributes/UPFHPSet.h"
#include "UPFCharacterStatSet.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFCharacterStatSet : public UUPFHPSet
{
	GENERATED_BODY()

public:
	UUPFCharacterStatSet();
	
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, Attack);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, AttackRange);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, MovementSpeed);
	ATTRIBUTE_ACCESSORS(UUPFCharacterStatSet, Defense);

private:
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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
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
};
