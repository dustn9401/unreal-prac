// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GameplayAbilities/UPFGameplayAbility_ADS.h"

#include "UPFGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/UPFCharacterBase.h"

UUPFGameplayAbility_ADS::UUPFGameplayAbility_ADS()
{
}

void UUPFGameplayAbility_ADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (!OwnerCharacter)
	{
		OwnerCharacter = CastChecked<AUPFCharacterBase>(ActorInfo->OwnerActor);
	}

	OwnerCharacter->OnAimingStart();

	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UPFGameplayTags::Ability_Active_RangedWeapon_ADS);
	Task->EventReceived.AddDynamic(this, &UUPFGameplayAbility_ADS::OnGameplayEventReceived);
	Task->ReadyForActivation();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UUPFGameplayAbility_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	check(OwnerCharacter);
	OwnerCharacter->OnAimingEnd();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UUPFGameplayAbility_ADS::OnGameplayEventReceived(FGameplayEventData Payload)
{
	constexpr bool bReplicateEndAbility = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, false);
}
