// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UPFGameplayAbility.h"
#include "Engine/DataAsset.h"
#include "UPFAbilityData.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<UUPFGameplayAbility> AbilityClass;
};
