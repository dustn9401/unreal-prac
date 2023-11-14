// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Attributes/UPFCharacterStatSet.h"

#include "Net/UnrealNetwork.h"

UUPFCharacterStatSet::UUPFCharacterStatSet()
{
	
}

void UUPFCharacterStatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UUPFCharacterStatSet, CurrentHP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFCharacterStatSet, MaxHP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFCharacterStatSet, Attack, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFCharacterStatSet, AttackRange, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFCharacterStatSet, AttackSpeed, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFCharacterStatSet, MovementSpeed, COND_OwnerOnly, REPNOTIFY_Always);
}

void UUPFCharacterStatSet::OnRep_CurrentHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, CurrentHP, OldValue);
}

void UUPFCharacterStatSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, MaxHP, OldValue);
}

void UUPFCharacterStatSet::OnRep_Attack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, Attack, OldValue);
}

void UUPFCharacterStatSet::OnRep_AttackRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, AttackRange, OldValue);
}

void UUPFCharacterStatSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, AttackSpeed, OldValue);
}

void UUPFCharacterStatSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, MovementSpeed, OldValue);
}

FString UUPFCharacterStatSet::ToString() const
{
	return FString::Printf(TEXT("MaxHP=%f, Attack=%f"), MaxHP.GetCurrentValue(), Attack.GetCurrentValue());
}
