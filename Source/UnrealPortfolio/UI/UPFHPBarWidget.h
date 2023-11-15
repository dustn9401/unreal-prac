// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPFHPBarWidget.generated.h"

struct FGameplayEffectSpec;
class UUPFCharacterStatSet;
/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetData(const UUPFCharacterStatSet& StatSet);
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="UpdateHPBarInstantCpp"))
	void K2_UpdateHPBarInstant(float CurrentHP, float MaxHP);	// 연출 없이 프로그레스바 업데이트, 초기화 또는 MaxHP 변경 시 호출됨

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnTakeDamageCpp"))
	void K2_OnTakeDamage(float NewHP);	// 데미지 입었을 때 게이지 감소
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnHealingCpp"))
	void K2_OnHealing(float NewHP);	// 힐 받았을 때 게이지 채우기

	void OnTakeDamage(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);
	void OnHealing(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);
};
