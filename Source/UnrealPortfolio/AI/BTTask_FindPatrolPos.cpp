// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPatrolPos.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "UPFAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/UPFCharacterAIInterface.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Unused = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	IUPFCharacterAIInterface* AICharacter = Cast<IUPFCharacterAIInterface>(ControllingPawn);
	if (!AICharacter)
	{
		return EBTNodeResult::Failed;
	}

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	const FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);	// 스폰 위치
	const float PatrolRadius = AICharacter->GetAIPatrolRadius();	// 정찰범위
	if (FNavLocation NextPatrolPos; NavSystem->GetRandomPointInNavigableRadius(Origin, PatrolRadius, NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
