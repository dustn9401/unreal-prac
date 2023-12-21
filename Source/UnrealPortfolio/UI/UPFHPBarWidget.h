// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPFHPBarWidget.generated.h"

struct FGameplayEffectSpec;
class UUPFHPSet;
/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetData(const UUPFHPSet* StatSet);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="UpdateHPBarInstantCpp"))
	void K2_UpdateHPBarInstant(float CurrentHP, float MaxHP);	// 연출 없이 프로그레스바 업데이트, 초기화 또는 MaxHP 변경 시 호출됨
};
