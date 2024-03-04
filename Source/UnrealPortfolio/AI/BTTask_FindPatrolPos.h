// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/**
 * 다음 정찰 위치를 찾고 세팅하는 태스크
 */
UCLASS()
class UNREALPORTFOLIO_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
