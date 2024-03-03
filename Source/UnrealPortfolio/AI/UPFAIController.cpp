// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/UPFAIController.h"

#include "UPFAI.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AUPFAIController::AUPFAIController()
{
}

void AUPFAIController::RunAI()
{
	// 사용할 블랙보드 지정 및 행동트리 실행
	if (UBlackboardComponent* BlackboardPtr = Blackboard.Get(); UseBlackboard(BBAsset, BlackboardPtr))
	{
		// HomePos를 폰의 생성 위치로 지정
		Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());
		
		const bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

void AUPFAIController::StopAI()
{
	if (UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BTComponent->StopTree();
	}
}

void AUPFAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}
