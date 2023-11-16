// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UPFHPBarWidget.h"

#include "Ability/Attributes/UPFCharacterStatSet.h"

void UUPFHPBarWidget::SetData(const UUPFCharacterStatSet* StatSet)
{
	K2_UpdateHPBarInstant(StatSet->GetCurrentHP(), StatSet->GetMaxHP());
	StatSet->OnTakeDamage.AddUObject(this, &UUPFHPBarWidget::OnTakeDamage);
	StatSet->OnHealing.AddUObject(this, &UUPFHPBarWidget::OnHealing);
	StatSet->OnMaxHPChanged.AddUObject(this, &UUPFHPBarWidget::K2_UpdateHPBarInstant);
}

void UUPFHPBarWidget::OnTakeDamage(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	K2_OnTakeDamage(NewValue);
}

void UUPFHPBarWidget::OnHealing(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	K2_OnHealing(NewValue);
}
