// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "UPFDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UUPFDamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
