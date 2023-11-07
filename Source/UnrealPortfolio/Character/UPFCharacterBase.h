// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPFCharacterBase.generated.h"

UCLASS()
class UNREALPORTFOLIO_API AUPFCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPFCharacterBase(const FObjectInitializer& ObjectInitializer);
};