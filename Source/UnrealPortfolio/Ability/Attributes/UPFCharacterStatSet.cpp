// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Attributes/UPFCharacterStatSet.h"

#include "GameplayEffectExtension.h"
#include "UnrealPortfolio.h"
#include "Net/UnrealNetwork.h"

UUPFCharacterStatSet::UUPFCharacterStatSet()
{
	bIsOnHPZeroInvoked = false;
}

void UUPFCharacterStatSet::OnInit()
{
	Super::OnInit();

	bIsOnHPZeroInvoked = false;
	
	InitCurrentHP(GetMaxHP());	// CurrentHP 값은 테이블에 없기 때문에, 여기서 MaxHP값으로 초기화
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
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFCharacterStatSet, Defense, COND_OwnerOnly, REPNOTIFY_Always);
}

bool UUPFCharacterStatSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// 서버전용
	// UPF_LOG_ATTRIBUTE(LogTemp, Log, TEXT("Called"));
	PrevHP = GetCurrentHP();
	PrevMaxHP = GetMaxHP();
	
	return Super::PreGameplayEffectExecute(Data);
}

void UUPFCharacterStatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 서버전용
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (Data.EvaluatedData.Attribute == GetCurrentHPAttribute())
	{
		const bool DidTakeDamage = PrevHP > GetCurrentHP();		// Data.EvaluatedData.Magnitude < 0.0f
		const bool DidHealing = PrevHP < GetCurrentHP();		// Data.EvaluatedData.Magnitude > 0.0f
		
		if (DidTakeDamage) 
		{
			OnTakeDamage.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PrevHP, GetCurrentHP());
			if (!bIsOnHPZeroInvoked && GetCurrentHP() <= 0.0f)
			{
				bIsOnHPZeroInvoked = true;
				OnHPZero.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PrevHP, GetCurrentHP());
			}
		}
		
		if (DidHealing)
		{
			OnHealing.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, PrevHP, GetCurrentHP());
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHPAttribute())
	{
		OnMaxHPChanged.Broadcast(GetCurrentHP(), GetMaxHP());
	}
}

void UUPFCharacterStatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UUPFCharacterStatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UUPFCharacterStatSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	// if (const AActor* OwningActor = GetOwningActor())
	// {
	// 	UPF_LOG_ATTRIBUTE(LogTemp, Log, TEXT("Called: %s, %s, %f -> %f (%f)"), *OwningActor->GetName(), *Attribute.AttributeName, OldValue, NewValue, GetCurrentHP());
	// }
}

void UUPFCharacterStatSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// if (const AActor* OwningActor = GetOwningActor())
	// {
	// 	UPF_LOG_ATTRIBUTE(LogTemp, Log, TEXT("Called: %s, %s, %f -> %f (%f)"), *OwningActor->GetName(), *Attribute.AttributeName, OldValue, NewValue, GetCurrentHP());
	// }

	if (Attribute == GetMaxHPAttribute())
	{
		// 최대 체력 감소 시 현재 체력 조절하는 부분
		if (GetCurrentHP() > NewValue)
		{
			if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
			{
				ASC->ApplyModToAttribute(GetCurrentHPAttribute(), EGameplayModOp::Override, NewValue);
			}
		}
	}
}

void UUPFCharacterStatSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
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

void UUPFCharacterStatSet::OnRep_CurrentHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, CurrentHP, OldValue);

	const float CurHP = GetCurrentHP();
	const float EstimatedMagnitude = CurHP - OldValue.GetCurrentValue();
	UPF_LOG_ATTRIBUTE(LogTemp, Log, TEXT("Called, %f -> %f"), OldValue.GetCurrentValue(), CurHP);

	if (EstimatedMagnitude < 0.0f)
	{
		OnTakeDamage.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurHP);
	}
	else if (EstimatedMagnitude > 0.0f)
	{
		OnHealing.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurHP);
	}

	if (!bIsOnHPZeroInvoked && CurHP <= 0.0f)
	{
		OnHPZero.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurHP);
	}

	bIsOnHPZeroInvoked = CurHP <= 0.0f;
}

void UUPFCharacterStatSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, MaxHP, OldValue);

	UPF_LOG_ATTRIBUTE(LogTemp, Log, TEXT("Called, %f -> %f"), OldValue.GetCurrentValue(), GetMaxHP());
	OnMaxHPChanged.Broadcast(GetCurrentHP(), GetMaxHP());
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

void UUPFCharacterStatSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFCharacterStatSet, Defense, OldValue);
}

FString UUPFCharacterStatSet::ToString() const
{
	return FString::Printf(TEXT("MaxHP=%f, Attack=%f"), MaxHP.GetCurrentValue(), Attack.GetCurrentValue());
}
