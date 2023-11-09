// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFGameplayAbility.h"

UUPFGameplayAbility::UUPFGameplayAbility(const FObjectInitializer& ObjectInitializer)
{
	
}

bool UUPFGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) &&
		ActorInfo->GetAnimInstance();
}
