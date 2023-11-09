// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFGameplayAbility_MeleeAttack.h"

UUPFGameplayAbility_MeleeAttack::UUPFGameplayAbility_MeleeAttack(const FObjectInitializer& ObjectInitializer)
{
	
}

void UUPFGameplayAbility_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Log, TEXT("UUPFGameplayAbility_MeleeAttack::ActivateAbility"));

	ensure(CurrentCombo == 0);
	CurrentCombo++;
	
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ComboTimer.Invalidate();

	// todo: AnimInstance 가져와서 몽타주 재생, EndDelegate 추가하기, AnimNotify 받아서 CheckCombo 구현하기
}

void UUPFGameplayAbility_MeleeAttack::CheckCombo()
{
}
