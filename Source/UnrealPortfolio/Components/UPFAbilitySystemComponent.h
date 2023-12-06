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
	virtual void AbilityLocalInputPressed(int32 InputID) override;
	virtual void AbilityLocalInputReleased(int32 InputID) override;
	// void OnGiveAbilitySet(const UUPFAbilitySet* AbilitySet);
	// void OnRemoveAbilitySet(const UUPFAbilitySet* AbilitySet);
};
