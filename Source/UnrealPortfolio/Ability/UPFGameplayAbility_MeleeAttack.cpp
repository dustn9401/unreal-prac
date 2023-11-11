// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFGameplayAbility_MeleeAttack.h"

#include "UPFGameplayTags.h"
#include "DataAssets/ComboAttackData.h"
#include "GameFramework/CharacterMovementComponent.h"

UUPFGameplayAbility_MeleeAttack::UUPFGameplayAbility_MeleeAttack(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UComboAttackData> ComboAttackDataRef(TEXT("/Script/UnrealPortfolio.ComboAttackData'/Game/UnrealPortfolio/DataAssets/DefaultComboAttackData.DefaultComboAttackData'"));
	if (ComboAttackDataRef.Object)
	{
		ComboAttackData = ComboAttackDataRef.Object;
	}

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = UPFGameplayTags::InputTag_Ability;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UUPFGameplayAbility_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Log, TEXT("UUPFGameplayAbility_MeleeAttack::ActivateAbility, %s"), *ActorInfo->AvatarActor->GetName());
	if (CurrentCombo != 0) return;

	// 콤보관련 변수 초기화
	CurrentCombo = 1;
	ComboTimer.Invalidate();

	// 몽타주 재생 중에는 이동을 막는다.
	if (UCharacterMovementComponent* CMC = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent))
	{
		CMC->SetMovementMode(MOVE_None);
	}
	
	CommitAbility(Handle, ActorInfo, ActivationInfo);
	
	UAnimInstance* AnimInst = ActorInfo->GetAnimInstance();
	check(AnimInst);
	
	// 몽타주 실행
	UE_LOG(LogTemp, Log, TEXT("UUPFGameplayAbility_MeleeAttack::Montage_Play"));
	AnimInst->Montage_Play(ComboAttackData->Montage, 1.0f);

	// 몽타주 종료 콜백
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UUPFGameplayAbility_MeleeAttack::OnMontageEnd);
	AnimInst->Montage_SetEndDelegate(EndDelegate, ComboAttackData->Montage);

	// 첫 콤보 체크 타이머는 수동으로 호출
	SetNextComboTimerIfPossible();
}

void UUPFGameplayAbility_MeleeAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (ComboTimer.IsValid())
	{
		HasNextComboCommand = true;
	}
}

void UUPFGameplayAbility_MeleeAttack::ProcessNextCombo()
{
	
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboAttackData->MaxComboCount);
	const FName NextSectionName = *FString::Printf(TEXT("Combo%d"), CurrentCombo - 1);

	UAnimInstance* AnimInst = CurrentActorInfo->GetAnimInstance();
	check(AnimInst);
	
	// JumpToSection() 으로 현재 콤보의 남은 애니메이션을 생략하고 다음콤보로 바로 이동
	AnimInst->Montage_JumpToSection(NextSectionName, ComboAttackData->Montage);
		
	SetNextComboTimerIfPossible();
}

void UUPFGameplayAbility_MeleeAttack::OnMontageEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	UE_LOG(LogTemp, Log, TEXT("UUPFGameplayAbility_MeleeAttack::OnMontageEnd"));
	
	if (UCharacterMovementComponent* CMC = Cast<UCharacterMovementComponent>(CurrentActorInfo->MovementComponent))
	{
		CMC->SetMovementMode(MOVE_Walking);
	}

	CurrentCombo = 0;
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UUPFGameplayAbility_MeleeAttack::SetNextComboTimerIfPossible()
{
	ComboTimer.Invalidate();

	const int32 ComboIndex = CurrentCombo - 1;
	check(ComboAttackData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	// ComboEffectiveTime 내에 공격 인풋이 들어와야 다음 콤보로 넘어갈 수 있도록 하는 로직
	float ComboEffectiveTime = (ComboAttackData->EffectiveFrameCount[ComboIndex] / ComboAttackData->FrameRate);
	if (ComboEffectiveTime > 0.0f)	// ComboIndex < ComboAttackData->EffectiveFrameCount.Num() - 1
	{
		const FTimerDelegate TimerCallback = FTimerDelegate::CreateWeakLambda(this, [this]
		{
			UE_LOG(LogTemp, Log, TEXT("UUPFGameplayAbility_MeleeAttack::TimerCallback, HasNextComboCommand=%d"), HasNextComboCommand);
			if (HasNextComboCommand)
			{
				HasNextComboCommand = false;
				ProcessNextCombo();
			}
		});
		
		GetWorld()->GetTimerManager().SetTimer(ComboTimer, TimerCallback, ComboEffectiveTime, false);
	}
}
