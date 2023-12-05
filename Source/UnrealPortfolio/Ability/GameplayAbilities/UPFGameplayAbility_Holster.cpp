// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GameplayAbilities/UPFGameplayAbility_Holster.h"

#include "AbilitySystemComponent.h"
#include "Components/UPFCharacterEquipmentComponent.h"
#include "GameFramework/Character.h"

UUPFGameplayAbility_Holster::UUPFGameplayAbility_Holster()
{
}

void UUPFGameplayAbility_Holster::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Log, TEXT("UUPFGameplayAbility_Holster::ActivateAbility"));
	
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	const TWeakObjectPtr<UAbilitySystemComponent> ASC = ActorInfo->AbilitySystemComponent;
	
	// 몽타주 실행은 이 함수로 해야 리플리케이션이 정상적으로 처리됨
	ASC->PlayMontage(this, ActivationInfo, HolsterMontage, 1.0f);
	
	// 몽타주 종료 콜백
	FOnMontageEnded MontageEndDelegate;
	MontageEndDelegate.BindUObject(this, &UUPFGameplayAbility_Holster::OnMontageEnd);
	ActorInfo->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, HolsterMontage);
}

void UUPFGameplayAbility_Holster::OnMontageEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
