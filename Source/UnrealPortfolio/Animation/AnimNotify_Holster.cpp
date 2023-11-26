// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_Holster.h"

void UAnimNotify_Holster::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	
}
