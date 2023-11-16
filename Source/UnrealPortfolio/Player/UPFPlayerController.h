// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UPFPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUPFPlayerController();
	
	UFUNCTION(BlueprintImplementableEvent, Category=Game, DisplayName="OnGameOverCpp")
	void K2_OnGameOver();
};
