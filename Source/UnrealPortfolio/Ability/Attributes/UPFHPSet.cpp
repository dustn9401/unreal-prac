// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Attributes/UPFHPSet.h"

#include "GameplayEffectExtension.h"
#include "UnrealPortfolio.h"
#include "Net/UnrealNetwork.h"

UUPFHPSet::UUPFHPSet(): CurrentHP(0), MaxHP(1.0f)
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

	DOREPLIFETIME(UUPFHPSet, CurrentHP);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFHPSet, MaxHP, COND_None, REPNOTIFY_Always);
}

void UUPFHPSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 서버전용
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;
	if (Attr == GetDamageAttribute())
	{
		SetCurrentHP(CurrentHP - GetDamage());
		SetDamage(0.0f);
	}
	else if (Attr == GetMaxHPAttribute())
	{
		OnMaxHPChanged.Broadcast(CurrentHP, GetMaxHP());
	}
	else
	{
		UPF_LOG_ATTRIBUTE(LogTemp, Error, TEXT("허용되지 않은 Attribute: %s"), *Attr.GetName());
	}

	if (!bIsOnHPZeroInvoked && CurrentHP <= 0.0f)
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
	if (Attribute == GetMaxHPAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UUPFHPSet::OnRep_CurrentHP() const
{
	OnCurrentHPChanged.Broadcast(CurrentHP, GetMaxHP());

	if (!bIsOnHPZeroInvoked && CurrentHP <= 0.0f)
	{
		bIsOnHPZeroInvoked = true;
		OnHPZero.Broadcast();
	}
}

void UUPFHPSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFHPSet, MaxHP, OldValue);
	
	OnMaxHPChanged.Broadcast(CurrentHP, GetMaxHP());
}
