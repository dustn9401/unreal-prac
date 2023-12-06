// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GameplayAbilities/UPFGameplayAbility_Holster.h"
#include "AbilitySystemComponent.h"
#include "UPFGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/UPFCharacterBase.h"
#include "Components/UPFCharacterEquipmentComponent.h"

UUPFGameplayAbility_Holster::UUPFGameplayAbility_Holster()
{
}

void UUPFGameplayAbility_Holster::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	const TWeakObjectPtr<UAbilitySystemComponent> ASC = ActorInfo->AbilitySystemComponent;
	
	// 몽타주 실행은 AnimInstance 가 아닌 ASC 의 함수로 실행한다.
	ASC->PlayMontage(this, ActivationInfo, HolsterMontage, 1.0f);
	
	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UPFGameplayTags::Ability_Active_Holster);
	Task->EventReceived.AddDynamic(this, &UUPFGameplayAbility_Holster::OnEventReceived);

	// C++ 에서는 아래 함수로 태스크를 실행시킨다.
	Task->ReadyForActivation();
	// Task->Activate();
}

void UUPFGameplayAbility_Holster::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 수납 중 인풋이 또 들어오면 수납 취소
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UUPFGameplayAbility_Holster::OnEventReceived(FGameplayEventData Payload)
{
	if (CurrentActorInfo == nullptr) return;
	
	AUPFCharacterBase* UPFCharacter = CastChecked<AUPFCharacterBase>(CurrentActorInfo->OwnerActor);
	UPFCharacter->EquipmentComponent->ToggleHolsterWeapon();

	// 여기서 어빌리티를 종료시켜서 다시 인풋이 들어왔을 때 취소 시키지 않는다.
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UUPFGameplayAbility_Holster::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                bool bReplicateCancelAbility)
{
	ActorInfo->AbilitySystemComponent->StopMontageIfCurrent(*HolsterMontage);
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
