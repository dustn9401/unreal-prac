// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/UPFGameplayAbility.h"
#include "UPFGameplayAbility_MeleeAttack.generated.h"

/**
 * 코스트가 없는 일반 근접 공격
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFGameplayAbility_MeleeAttack : public UUPFGameplayAbility
{
	GENERATED_BODY()

// overrides
public:
	UUPFGameplayAbility_MeleeAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

// combo
protected:
	void CheckCombo();	// 콤보 허용 타이밍 내에 공격 인풋이 들어왔는지 확인하여 HasNextComboCommand값을 세팅하는 함수
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=DataAsset, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UComboAttackData> ComboAttackData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Montage)
	TObjectPtr<class UAnimMontage> AnimMontage;

	int32 CurrentCombo = 0;		// 콤보중이 아닐때 0, 콤보중인경우 1/2/3/4
	FTimerHandle ComboTimer;	// 콤보 연속/종료 판정을 위한 타이머
	bool HasNextComboCommand = false;	// 다음 콤보로 이어져야 하는지 여부 플래그
};
