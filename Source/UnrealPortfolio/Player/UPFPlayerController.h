// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interface/CameraShakeHandler.h"
#include "UPFPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFPlayerController : public APlayerController, public ICameraShakeHandler
{
	GENERATED_BODY()

public:
	AUPFPlayerController();
	
	UFUNCTION(BlueprintImplementableEvent, Category=Game, DisplayName="OnGameOverCpp")
	void K2_OnGameOver();

// ICameraShakeHandler Impl
public:
	virtual void PlayCameraShake(const TSubclassOf<UCameraShakeBase> CameraShakeClass) override;
};
