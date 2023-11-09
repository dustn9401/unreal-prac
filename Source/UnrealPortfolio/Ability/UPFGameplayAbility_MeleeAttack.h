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
	void ProcessNextCombo();	// 다음 콤보 몽타주 실행 및 관련 변수 업데이트
	void OnMontageEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);	// 몽타주 콜백용 함수
	void SetNextComboTimerIfPossible();		// 마지막 콤보가 아니고 입력이 제대로 들어 왔다면 다음 콤보 판정을 위한 타이머 세팅
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=DataAsset, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UComboAttackData> ComboAttackData;

	int32 CurrentCombo = 0;		// 콤보중이 아닐때 0, 콤보중인경우 1/2/3/4
	FTimerHandle ComboTimer;	// 콤보 연속/종료 판정을 위한 타이머
	bool HasNextComboCommand = false;	// 다음 콤보로 이어져야 하는지 여부 플래그
};
