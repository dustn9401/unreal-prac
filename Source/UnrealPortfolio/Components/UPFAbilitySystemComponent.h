// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UPFAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void SetupPlayerInputComponent(UEnhancedInputComponent* InputComponent);
	void OnTaggedInputPressed(const FGameplayTag& InputTag);
	void OnTaggedInputReleased(const FGameplayTag& InputTag);
};
