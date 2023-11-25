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

	// 어빌리티 발동 중 인풋 들어왔을때 호출되는 함수, 콤보 판정 처리
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

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

// hit check
public:
	// 몽타주 Notify
	void OnAnimNotify();

protected:
	// Hit 계산은 클라에서 이루어 지고, 서버에 HitResults를 보내 검증하기위한 함수
	// 이 함수에서 검증 완료 된 HitResult를 대상으로 HitConfirm 함수를 호출해 줘야 실제로 데미지를 입히기 위한 GameplayEffect 로직이 진행됨
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCNotifyHit(const TArray<FHitResult>& OutHitResults, float HitCheckTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCNotifyMiss(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime);

	// 서버에서 ServerRPCNotifyHit 함수를 통해 검증 완료 된 HitResult를 대상으로 Effect 적용
	void HitConfirm(const FHitResult& HitResult);

	float AttackTime = 1.4667f;
	float LastAttackStartTime = 0.0f;	// 마지막으로 공격한 시간 기록용
	float AttackTimeDifference = 0.0f;	// 서버와의 공격 시간 차이 기록용

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass;
};