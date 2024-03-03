// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UPFAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUPFAIController();

	void RunAI();
	void StopAI();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlackboardData> BBAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BTAsset;
};
