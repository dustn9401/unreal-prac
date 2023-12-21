// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Attributes/UPFHPSet.h"

#include "GameplayEffectExtension.h"
#include "UnrealPortfolio.h"
#include "Net/UnrealNetwork.h"

UUPFHPSet::UUPFHPSet()
{
	bIsOnHPZeroInvoked = false;
}

void UUPFHPSet::OnBeginPlay()
{
	Super::OnBeginPlay();

	bIsOnHPZeroInvoked = false;
}

void UUPFHPSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UUPFHPSet, CurrentHP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFHPSet, MaxHP, COND_None, REPNOTIFY_Always);
}

bool UUPFHPSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// 서버전용
	// UPF_LOG_ATTRIBUTE(LogTemp, Log, TEXT("Called"));
	PrevHP = GetCurrentHP();
	PrevMaxHP = GetMaxHP();
	
	return Super::PreGameplayEffectExecute(Data);
}

void UUPFHPSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 서버전용
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentHPAttribute())
	{
		OnCurrentHPChanged.Broadcast(GetCurrentHP(), GetMaxHP());
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHPAttribute())
	{
		OnMaxHPChanged.Broadcast(GetCurrentHP(), GetMaxHP());
	}

	if (!bIsOnHPZeroInvoked && GetCurrentHP() <= 0.0f)
	{
		bIsOnHPZeroInvoked = true;
		OnHPZero.Broadcast();
	}
}

void UUPFHPSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}


void UUPFHPSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetCurrentHPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHP());
	}
	else if (Attribute == GetMaxHPAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UUPFHPSet::OnRep_CurrentHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFHPSet, CurrentHP, OldValue);
	
	OnCurrentHPChanged.Broadcast(GetCurrentHP(), GetMaxHP());

	if (!bIsOnHPZeroInvoked && GetCurrentHP() <= 0.0f)
	{
		bIsOnHPZeroInvoked = true;
		OnHPZero.Broadcast();
	}
}

void UUPFHPSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFHPSet, MaxHP, OldValue);
	
	OnMaxHPChanged.Broadcast(GetCurrentHP(), GetMaxHP());
}
