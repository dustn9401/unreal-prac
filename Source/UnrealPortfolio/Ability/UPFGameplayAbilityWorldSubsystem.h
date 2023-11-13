// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UPFGameplayAbilityWorldSubsystem.generated.h"

/**
 * 어빌리티 시스템에서 사용하는 공통 기능 모음
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFGameplayAbilityWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void PostInitialize() override;
};
