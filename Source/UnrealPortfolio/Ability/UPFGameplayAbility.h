// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UPFGameplayAbility.generated.h"
/**
 * 모든 어빌리티들의 base class
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UUPFGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

// overrides
public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

protected:
	AController* GetControllerFromActorInfoRecursive() const;
};
