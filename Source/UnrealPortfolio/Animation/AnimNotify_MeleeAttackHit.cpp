// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_MeleeAttackHit.h"

#include "Interface/AttackAnimationInterface.h"

void UAnimNotify_MeleeAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (const auto AttackPawn = Cast<IMeleeAttackAnimationInterface>(MeshComp->GetOwner()); AttackPawn)
		{
			AttackPawn->OnMeleeAttackAnimationHit();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MeshComp->GetOwner() is NOT IAttackAnimationInterface!!"));
		}
	}
}
