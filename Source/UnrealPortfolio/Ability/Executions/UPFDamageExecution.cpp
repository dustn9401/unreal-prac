// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Executions/UPFDamageExecution.h"

#include "Ability/Attributes/UPFCharacterStatSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UUPFCharacterStatSet::GetAttackAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UUPFDamageExecution::UUPFDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
}

void UUPFDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UE_LOG(LogTemp, Log, TEXT("UUPFDamageExecution::Execute_Implementation"));
}
