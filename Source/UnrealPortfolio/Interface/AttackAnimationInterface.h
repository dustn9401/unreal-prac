// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackAnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMeleeAttackAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 근접공격 애니메이션 Notify를 받기 위한 인터페이스
 */
class UNREALPORTFOLIO_API IMeleeAttackAnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 근접 공격 모션 중, 딱 가격하는 지점에서 호출됨
	virtual void OnMeleeAttackAnimationHit() = 0;
};
