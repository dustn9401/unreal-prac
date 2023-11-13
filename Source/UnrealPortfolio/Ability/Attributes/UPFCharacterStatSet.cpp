// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Attributes/UPFCharacterStatSet.h"

UUPFCharacterStatSet::UUPFCharacterStatSet()
{
	
}

void UUPFCharacterStatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UUPFCharacterStatSet::OnRep_CurrentHP(const FGameplayAttributeData& OldValue)
{
}

void UUPFCharacterStatSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue)
{
}

void UUPFCharacterStatSet::OnRep_Attack(const FGameplayAttributeData& OldValue)
{
}

void UUPFCharacterStatSet::OnRep_AttackRange(const FGameplayAttributeData& OldValue)
{
}

void UUPFCharacterStatSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
}

void UUPFCharacterStatSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
}
