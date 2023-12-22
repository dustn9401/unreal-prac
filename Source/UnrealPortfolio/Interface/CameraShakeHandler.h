// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraShakeHandler.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCameraShakeHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 플레이어의 카메라 흔들림 기능을 구현할 오브젝트에 붙이는 인터페이스
 */
class UNREALPORTFOLIO_API ICameraShakeHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void PlayCameraShake(const TSubclassOf<UCameraShakeBase> CameraShakeClass) = 0;
};
