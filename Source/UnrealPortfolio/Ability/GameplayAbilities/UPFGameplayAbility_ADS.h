// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/UPFGameplayAbility_RangedWeapon.h"
#include "UPFGameplayAbility_ADS.generated.h"

class AUPFCharacterBase;
/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFGameplayAbility_ADS : public UUPFGameplayAbility_RangedWeapon
{
	GENERATED_BODY()

public:
	UUPFGameplayAbility_ADS();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnGameplayEventReceived(FGameplayEventData Payload);

private:
	UPROPERTY()
	TObjectPtr<AUPFCharacterBase> OwnerCharacter;
};
