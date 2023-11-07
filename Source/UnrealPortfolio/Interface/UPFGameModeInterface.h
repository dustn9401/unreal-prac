// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UPFGameModeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUPFGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * GameMode에 접근하기 위한 인터페이스. GameMode에만 구현할 것!
 */
class UNREALPORTFOLIO_API IUPFGameModeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnPlayerDead() = 0;
};
