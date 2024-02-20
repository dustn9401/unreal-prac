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

	UFUNCTION(BlueprintImplementableEvent, Category=Game, DisplayName="OnGameClearCpp")
	void K2_OnGameClear();

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION(BlueprintCallable)
	void QuitLevel();

	virtual void UpdateRotation(float DeltaTime) override;

// ICameraShakeHandler Impl
public:
	virtual void PlayCameraShake(const TSubclassOf<UCameraShakeBase> CameraShakeClass) override;
};
