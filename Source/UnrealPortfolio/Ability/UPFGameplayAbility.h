// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UPFGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UUPFGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
