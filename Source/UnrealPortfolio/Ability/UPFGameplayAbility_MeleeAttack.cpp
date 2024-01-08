// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFGameplayAbility_MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "UPFGameplayTags.h"
#include "Attributes/UPFStatSet.h"
#include "Character/UPFCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/UPFCharacterEquipmentComponent.h"
#include "Components/UPFWeaponStateComponent.h"
#include "DataAssets/ComboAttackData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Physics/UPFCollision.h"

UUPFGameplayAbility_MeleeAttack::UUPFGameplayAbility_MeleeAttack(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UComboAttackData> ComboAttackDataRef(TEXT("/Script/UnrealPortfolio.ComboAttackData'/Game/UnrealPortfolio/DataAssets/DefaultComboAttackData.DefaultComboAttackData'"));
	if (ComboAttackDataRef.Object)
	{
		ComboAttackData = ComboAttackDataRef.Object;
	}

	// FAbilityTriggerData TriggerData;
	// TriggerData.TriggerTag = UPFGameplayTags::InputTag_Ability;
	// TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	// AbilityTriggers.Add(TriggerData);
}

bool UUPFGameplayAbility_MeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

	AUPFCharacterBase* UPFCharacter = Cast<AUPFCharacterBase>(ActorInfo->OwnerActor);
	if (!UPFCharacter) return false;

	// 원거리 무기를 들고 있으면, 발동 불가
	if (!UPFCharacter->EquipmentComponent->GetIsHolstered()) return false;

	return true;
}

void UUPFGameplayAbility_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	if (CurrentCombo != 0) return;

	// 콤보관련 변수 초기화
	CurrentCombo = 1;
	ComboTimer.Invalidate();
	
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 몽타주 재생 중에는 이동을 막는다.
	if (UCharacterMovementComponent* CMC = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent))
	{
		CMC->SetMovementMode(MOVE_None);
	}
	
	// 몽타주 실행
	const TWeakObjectPtr<UAbilitySystemComponent> ASC = ActorInfo->AbilitySystemComponent;
	ASC->PlayMontage(this, ActivationInfo, ComboAttackData->Montage, 1.0f);

	/*
	 * 어빌리티가 종료되는 시점은 로컬 컨트롤러의 인풋에 의해 달라짐
	 * AnimNotify 를 로컬 컨트롤러만 수신하여 타겟 계산하여 서버에 전달
	 */
	if (IsLocallyControlled())
	{
		// 몽타주 종료 콜백
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UUPFGameplayAbility_MeleeAttack::OnMontageEnd);
		ActorInfo->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, ComboAttackData->Montage);

		// 첫 콤보 체크 타이머는 수동으로 호출
		SetNextComboTimerIfPossible();
	}
	// 나머지는 TargetData 가 Set 될때의 이벤트를 등록한다.
	else
	{
		TargetDataReadyDelegateHandle = ActorInfo->AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey())
			.AddUObject(this, &UUPFGameplayAbility_MeleeAttack::OnTargetDataReadyCallback);
	}
}

void UUPFGameplayAbility_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ensure(CurrentActorInfo))
	{
		if (UCharacterMovementComponent* CMC = Cast<UCharacterMovementComponent>(CurrentActorInfo->MovementComponent))
		{
			CMC->SetMovementMode(MOVE_Walking);
		}
		
		if (TargetDataReadyDelegateHandle.IsValid())
		{
			UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
			check(ASC);
			
			ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(TargetDataReadyDelegateHandle);
			ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
			TargetDataReadyDelegateHandle.Reset();
		}
	}
	
	CurrentCombo = 0;
	ComboTimer.Invalidate();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
	if (!ensure(CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())) return;
	
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboAttackData->MaxComboCount);
	const FName NextSectionName = *FString::Printf(TEXT("Combo%d"), CurrentCombo - 1);

	// 로컬 컨트롤러에서만 호출되는 함수이기 때문에, 현재 몽타주가 실행중임이 보장되어야 함
	if (!ensure(GetCurrentMontage() == ComboAttackData->Montage)) return;
	
	// 현재 콤보의 남은 애니메이션을 생략하고 다음콤보로 바로 이동
	CurrentActorInfo->AbilitySystemComponent->CurrentMontageJumpToSection(NextSectionName);
		
	SetNextComboTimerIfPossible();
}

void UUPFGameplayAbility_MeleeAttack::OnMontageEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	// Montage 종료 콜백이 로컬 컨트롤러만 등록되어 있기 때문에, replicate 해야함
	constexpr bool bReplicateEndAbility = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, false);
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
			if (HasNextComboCommand)
			{
				HasNextComboCommand = false;
				ProcessNextCombo();
			}
		});
		
		GetWorld()->GetTimerManager().SetTimer(ComboTimer, TimerCallback, ComboEffectiveTime, false);
	}
}

void UUPFGameplayAbility_MeleeAttack::OnAnimNotify()
{
	// 로컬 컨트롤러 에서만 호출되는 함수
	// 이 함수에서 콜리전 Hit 계산을 수행한 뒤, 서버 RPC를 호출한다.
	
	if (!ensure(CurrentActorInfo->IsLocallyControlled())) return;
	check(CurrentActorInfo && CurrentActorInfo->OwnerActor.IsValid() && CurrentActorInfo->AbilitySystemComponent.IsValid());

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	AUPFCharacterBase* Instigator = CastChecked<AUPFCharacterBase>(CurrentActorInfo->OwnerActor);
	
	TArray<FHitResult> OutHitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, Instigator);
	// Params.IgnoreMask = static_cast<FMaskFilter>(TeamType);	// TODO: 피아식별
	
	const float AttackRange = ASC->GetNumericAttribute(UUPFStatSet::GetAttackRangeAttribute());
	const float AttackRadius = 50.0f;	// TODO: 무기 종류에 따라 값 설정
	// const float AttackDamage = ASC->GetNumericAttribute(UUPFStatSet::GetAttackAttribute());
	
	const FVector Forward = Instigator->GetActorForwardVector();
	const FVector Start = Instigator->GetActorLocation() + Forward * Instigator->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * AttackRange;

	// 근접공격은 여러명 타격 가능
	bool HitDetected = GetWorld()-> SweepMultiByChannel(OutHitResults, Start, End,
		FQuat::Identity,
		CCHANNEL_UPFACTION,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);
	
	float HitCheckTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	
	FGameplayAbilityTargetDataHandle TargetData;
	if (HitDetected)
	{
		for (const FHitResult& FoundHit : OutHitResults)
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;

			TargetData.Add(NewTargetData);
		}
	}
	
	FScopedPredictionWindow ScopedPrediction(ASC, CurrentActivationInfo.GetActivationPredictionKey());
	
	OnTargetDataReadyCallback(TargetData, FGameplayTag::EmptyTag);

	// 클라이언트인 경우, 서버에 TargetData 를 전달하는 함수를 호출해줘야 함
	if (!CurrentActorInfo->IsNetAuthority())
	{
		ASC->CallServerSetReplicatedTargetData(CurrentSpecHandle,
			CurrentActivationInfo.GetActivationPredictionKey(),
			TargetData,
			FGameplayTag::EmptyTag,
			ASC->ScopedPredictionKey);
	}
}

void UUPFGameplayAbility_MeleeAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	FScopedPredictionWindow ScopedPrediction(ASC);
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, InData, EffectClass, 1.0f);

	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}
