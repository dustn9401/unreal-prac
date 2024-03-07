// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Ability/UPFGameplayAbility.h"
#include "UPFGameplayAbility_Holster.generated.h"

/**
 * 현재 선택된 무기를 수납 또는 손에 착용하는 어빌리티
 * 무기를 수납한 상태에서는 무기 발사 또는 재장전 등 무기 어빌리티 발동 시
 * 이 어빌리티가 우선 호출되어 손에 쥐어진다.
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFGameplayAbility_Holster : public UUPFGameplayAbility
{
	GENERATED_BODY()

public:
	UUPFGameplayAbility_Holster();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// AnimNotify 에서 보내주는 GameplayEvent 에 의해 호출되는 함수, 소켓 변경 및 관련 변수업데이트 이후 호출되어야 함 
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> HolsterMontage;
};
