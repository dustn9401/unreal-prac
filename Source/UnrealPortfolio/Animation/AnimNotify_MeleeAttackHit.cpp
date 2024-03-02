// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_MeleeAttackHit.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_MeleeAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (AActor* OwnerActor = MeshComp->GetOwner())
		{
			FGameplayEventData Payload;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
		}
	}
}
