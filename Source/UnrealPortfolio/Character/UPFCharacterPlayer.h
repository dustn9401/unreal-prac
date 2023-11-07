// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/UPFCharacterBase.h"
#include "UPFCharacterPlayer.generated.h"

/**
 * 플레이어용 캐릭터
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFCharacterPlayer : public AUPFCharacterBase
{
	GENERATED_BODY()

public:
	AUPFCharacterPlayer(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
};
