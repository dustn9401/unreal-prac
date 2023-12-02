// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UPFHUD.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUPFHUD();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairTexture;

public:
	virtual void DrawHUD() override;
};
