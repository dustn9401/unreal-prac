// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/UPFCharacterBase.h"
#include "Interface/UPFCharacterAIInterface.h"
#include "UPFCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFCharacterNonPlayer : public AUPFCharacterBase, public IUPFCharacterAIInterface
{
	GENERATED_BODY()

public:
	AUPFCharacterNonPlayer(const FObjectInitializer& ObjectInitializer);

protected:
	virtual FName GetStatGroup() override;
	
	virtual void OnDeathStart() override;

	// IUPFCharacterAIInterface impl
public:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	FAICharacterAttackFinished OnAttackFinished;
	virtual void ActivateAbilityByAI(FGameplayTag AbilityTag) override;
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
};
