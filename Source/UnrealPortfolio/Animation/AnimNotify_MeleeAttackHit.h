// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MeleeAttackHit.generated.h"

/**
 * 근접 공격 애니메이션의 공격 판정 처리를 위한 Notify
 * 내용이 커질 수 있어서 코드로 구현
 */
UCLASS()
class UNREALPORTFOLIO_API UAnimNotify_MeleeAttackHit : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
};
