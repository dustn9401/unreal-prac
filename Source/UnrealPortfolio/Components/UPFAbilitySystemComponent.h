// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UPFAbilitySystemComponent.generated.h"

class UUPFAbilitySet;
/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UUPFAbilitySystemComponent();

public:
	void AbilityInputTagTriggered(FGameplayTag InputTag);
	void AbilityInputTagCompleted(FGameplayTag InputTag);

protected:
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
};
