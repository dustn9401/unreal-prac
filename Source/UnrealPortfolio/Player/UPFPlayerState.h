// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "UPFPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AUPFPlayerState(const FObjectInitializer& ObjectInitializer);
};
