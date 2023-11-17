// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Executions/UPFDamageExecution.h"

#include "Ability/Attributes/UPFCharacterStatSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition BaseDefenseDef;

	FDamageStatics()
	{
		// InSnapshot: GameplayEffectSpec이 생성될 때 어트리뷰트를 캡쳐할지 여부
		// true로 설정 시 PreAttributeChange() 가 호출되지 않게 된다.
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UUPFCharacterStatSet::GetAttackAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseDefenseDef = FGameplayEffectAttributeCaptureDefinition(UUPFCharacterStatSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().BaseDefenseDef);
}

void UUPFDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	// UE_LOG(LogTemp, Log, TEXT("UUPFDamageExecution::Execute_Implementation"));

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Instigator의 공격력
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	// Target의 방어력
	float BaseDefense = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDefenseDef, EvaluateParameters, BaseDefense);

	// 최종 데미지 감소율 계산
	constexpr float DamageReductionConstant = 300.0f;
	const float MultiplierByDefense = 1.0f - BaseDefense / (BaseDefense + DamageReductionConstant);

	if (const float FinalDamage = FMath::Max(0.0f, BaseDamage * MultiplierByDefense); FinalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UUPFCharacterStatSet::GetCurrentHPAttribute(), EGameplayModOp::Additive, -FinalDamage));
	}
}
