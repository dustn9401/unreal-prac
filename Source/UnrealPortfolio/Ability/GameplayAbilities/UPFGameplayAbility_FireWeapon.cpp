// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GameplayAbilities/UPFGameplayAbility_FireWeapon.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AIController.h"
#include "AIController.h"
#include "UnrealPortfolio.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/UPFCharacterBase.h"
#include "Item/ItemInstance/Equipments/UPFRangedWeaponInstance.h"
#include "Physics/UPFCollision.h"

namespace UPFConsoleVariables
{
	static float DrawBulletTracesDuration = 0.0f;
	static FAutoConsoleVariableRef CVarDrawBulletTraceDuration(
		TEXT("upf.Weapon.DrawBulletTraceDuration"),
		DrawBulletTracesDuration,
		TEXT("Should we do debug drawing for bullet traces (if above zero, sets how long (in seconds))"),
		ECVF_Default);

	static float DrawBulletHitDuration = 0.0f;
	static FAutoConsoleVariableRef CVarDrawBulletHits(
		TEXT("upf.Weapon.DrawBulletHitDuration"),
		DrawBulletHitDuration,
		TEXT("Should we do debug drawing for bullet impacts (if above zero, sets how long (in seconds))"),
		ECVF_Default);

	static float DrawBulletHitRadius = 3.0f;
	static FAutoConsoleVariableRef CVarDrawBulletHitRadius(
		TEXT("upf.Weapon.DrawBulletHitRadius"),
		DrawBulletHitRadius,
		TEXT("When bullet hit debug drawing is enabled (see DrawBulletHitDuration), how big should the hit radius be? (in uu)"),
		ECVF_Default);
}

UUPFGameplayAbility_FireWeapon::UUPFGameplayAbility_FireWeapon()
{
	MaxWalkSpeedOnFiringWeapon = 230.0f;

	TimerCallback.BindUObject(this, &UUPFGameplayAbility_FireWeapon::OnFinishWait);
}

bool UUPFGameplayAbility_FireWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                        const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AUPFRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	if (WeaponInstance == nullptr)
	{
		return false;
	}

	return WeaponInstance->GetMagazineAmmo() > 0;
}

void UUPFGameplayAbility_FireWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	// 애니메이션 재생
	ActorInfo->AbilitySystemComponent->PlayMontage(this, ActivationInfo, FireMontage, 1.0f);

	AUPFRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	check(WeaponInstance);
	
	UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, WeaponInstance->GetFireDelay());
	Task->OnFinish.AddDynamic(this, &UUPFGameplayAbility_FireWeapon::OnFinishWait);
	Task->ReadyForActivation();
	
	if (IsLocallyControlled())
	{
		// 로컬에서만 Trace 수행 후 서버에 Set TargetData 요청을 보낸다.
		StartRangedWeaponTargeting(ActorInfo);
	}
	else
	{
		// 나머지는 TargetData 가 Set 될때의 이벤트를 등록한다.
		OnTargetDataReadyCallbackDelegateHandle = ActorInfo->AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey())
			.AddUObject(this, &UUPFGameplayAbility_FireWeapon::OnTargetDataReadyCallback);
	}
}

void UUPFGameplayAbility_FireWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ensure(CurrentActorInfo))
	{
		// Target Set Delegate 바인딩 제거
		if (OnTargetDataReadyCallbackDelegateHandle.IsValid())
		{
			UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
			check(ASC);
			
			ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
			ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
			OnTargetDataReadyCallbackDelegateHandle.Reset();
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UUPFGameplayAbility_FireWeapon::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   FGameplayTagContainer* OptionalRelevantTags)
{
	if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		return false;
	}

	AUPFRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	if (WeaponInstance == nullptr) return false;
	if (WeaponInstance->GetMagazineAmmo() <= 0) return false;
	
	// WeaponInstance->ConsumeAmmo(1);
	// UPF_LOG_ABILITY(LogTemp, Log, TEXT("GetMagazineAmmo() = %d"), WeaponInstance->GetMagazineAmmo());
	
	return true;
}

void UUPFGameplayAbility_FireWeapon::OnFinishWait()
{
	constexpr bool bReplicateEndAbility = false;
	constexpr bool bWasCancelled = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

int32 UUPFGameplayAbility_FireWeapon::FindFirstPawnHitResult(const TArray<FHitResult>& HitResults)
{
	for (int32 Idx = 0; Idx < HitResults.Num(); ++Idx)
	{
		const FHitResult& CurHitResult = HitResults[Idx];
		if (CurHitResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass()))
		{
			// If we hit a pawn, we're good
			return Idx;
		}
		else
		{
			AActor* HitActor = CurHitResult.HitObjectHandle.FetchActor();
			if ((HitActor != nullptr) && (HitActor->GetAttachParentActor() != nullptr) && (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr))
			{
				// If we hit something attached to a pawn, we're good
				return Idx;
			}
		}
	}

	return INDEX_NONE;
}

void UUPFGameplayAbility_FireWeapon::StartRangedWeaponTargeting(const FGameplayAbilityActorInfo* ActorInfo)
{
	check(ActorInfo);
	
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	check(ASC);
	
	FScopedPredictionWindow ScopedPrediction(ASC, CurrentActivationInfo.GetActivationPredictionKey());

	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(/*out*/ FoundHits);
	
	FGameplayAbilityTargetDataHandle TargetData;
	if (FoundHits.Num() > 0)
	{
		for (const FHitResult& FoundHit : FoundHits)
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;
			TargetData.Add(NewTargetData);
		}
	}

	const FGameplayTag ApplicationTag = FGameplayTag::EmptyTag;
	
	// 로컬 컨트롤러는 Callback 을 즉시 호출
	OnTargetDataReadyCallback(TargetData, ApplicationTag);
	
	// 로컬컨트롤러가 서버가 아닌 경우, 데미지의 실제 적용은 서버의 OnTargetDataReadyCallback() 에서 이루어짐
	if (!ActorInfo->IsNetAuthority())
	{
		ASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), TargetData, ApplicationTag, ASC->ScopedPredictionKey);
	}
}

FHitResult UUPFGameplayAbility_FireWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const
{
	TArray<FHitResult> HitResults;
	
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;

	if (SweepRadius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, CCHANNEL_UPFACTION, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, CCHANNEL_UPFACTION, TraceParams);
	}

	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		// Filter the output list to prevent multiple hits on the same actor;
		// this is to prevent a single bullet dealing damage multiple times to
		// a single actor if using an overlap trace
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
			{
				return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
			};

			if (!OutHitResults.ContainsByPredicate(Pred))
			{
				OutHitResults.Add(CurHitResult);
			}
		}

		Hit = OutHitResults.Last();
	}
	else
	{
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	return Hit;
}

FHitResult UUPFGameplayAbility_FireWeapon::DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHits) const
{
#if ENABLE_DRAW_DEBUG
	if (UPFConsoleVariables::DrawBulletTracesDuration > 0.0f)
	{
		static float DebugThickness = 1.0f;
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, UPFConsoleVariables::DrawBulletTracesDuration, 0, DebugThickness);
	}
#endif // ENABLE_DRAW_DEBUG

	FHitResult Impact;

	// 우선 SweepRadius 를 0으로 주고 쿼리
	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		Impact = WeaponTrace(StartTrace, EndTrace, /*SweepRadius=*/ 0.0f, bIsSimulated, /*out*/ OutHits);
	}

	// SweepRadius 를 0으로 준 쿼리로 대상을 못찾았고 SweepRadius 가 있는 경우, 다시 쿼리한다. 
	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		if (SweepRadius > 0.0f)
		{
			TArray<FHitResult> SweepHits;
			Impact = WeaponTrace(StartTrace, EndTrace, SweepRadius, bIsSimulated, /*out*/ SweepHits);

			// If the trace with sweep radius enabled hit a pawn, check if we should use its hit results
			const int32 FirstPawnIdx = FindFirstPawnHitResult(SweepHits);
			if (SweepHits.IsValidIndex(FirstPawnIdx))
			{
				// If we had a blocking hit in our line trace that occurs in SweepHits before our
				// hit pawn, we should just use our initial hit results since the Pawn hit should be blocked
				bool bUseSweepHits = true;
				for (int32 Idx = 0; Idx < FirstPawnIdx; ++Idx)
				{
					const FHitResult& CurHitResult = SweepHits[Idx];

					auto Pred = [&CurHitResult](const FHitResult& Other)
					{
						return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
					};
					if (CurHitResult.bBlockingHit && OutHits.ContainsByPredicate(Pred))
					{
						bUseSweepHits = false;
						break;
					}
				}

				if (bUseSweepHits)
				{
					OutHits = SweepHits;
				}
			}
		}
	}

	return Impact;
}

FVector VRandConeNormalDistribution(const FVector& Dir, const float ConeHalfAngleRad, const float Exponent)
{
	if (ConeHalfAngleRad > 0.f)
	{
		const float ConeHalfAngleDegrees = FMath::RadiansToDegrees(ConeHalfAngleRad);

		// consider the cone a concatenation of two rotations. one "away" from the center line, and another "around" the circle
		// apply the exponent to the away-from-center rotation. a larger exponent will cluster points more tightly around the center
		const float FromCenter = FMath::Pow(FMath::FRand(), Exponent);
		const float AngleFromCenter = FromCenter * ConeHalfAngleDegrees;
		const float AngleAround = FMath::FRand() * 360.0f;

		FRotator Rot = Dir.Rotation();
		FQuat DirQuat(Rot);
		FQuat FromCenterQuat(FRotator(0.0f, AngleFromCenter, 0.0f));
		FQuat AroundQuat(FRotator(0.0f, 0.0, AngleAround));
		FQuat FinalDirectionQuat = DirQuat * AroundQuat * FromCenterQuat;
		FinalDirectionQuat.Normalize();

		return FinalDirectionQuat.RotateVector(FVector::ForwardVector);
	}
	else
	{
		return Dir.GetSafeNormal();
	}
}

void UUPFGameplayAbility_FireWeapon::TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, TArray<FHitResult>& OutHits)
{
	AUPFRangedWeaponInstance* WeaponData = InputData.WeaponInstance;
	check(WeaponData);

	const int32 BulletsPerCartridge = WeaponData->GetBulletsPerCartridge();

	for (int32 BulletIndex = 0; BulletIndex < BulletsPerCartridge; ++BulletIndex)
	{
		const float BaseSpreadAngle = WeaponData->GetCalculatedSpreadAngle();
		const float SpreadAngleMultiplier = WeaponData->GetCalculatedSpreadAngleMultiplier();
		const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

		const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

		const FVector BulletDir = VRandConeNormalDistribution(InputData.AimDir, HalfSpreadAngleInRadians, WeaponData->GetSpreadExponent());

		const FVector EndTrace = InputData.StartTrace + (BulletDir * WeaponData->GetMaxDamageRange());

		TArray<FHitResult> AllImpacts;

		FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, EndTrace, WeaponData->GetBulletTraceSweepRadius(), /*bIsSimulated=*/ false, /*out*/ AllImpacts);

		const AActor* HitActor = Impact.GetActor();

		if (HitActor)
		{
#if ENABLE_DRAW_DEBUG
			if (UPFConsoleVariables::DrawBulletHitDuration > 0.0f)
			{
				DrawDebugSphere(GetWorld(), Impact.ImpactPoint, UPFConsoleVariables::DrawBulletHitRadius, 3, FColor::Red, false, UPFConsoleVariables::DrawBulletHitDuration);
			}
#endif

			if (AllImpacts.Num() > 0)
			{
				OutHits.Append(AllImpacts);
			}
		}

		// 아무것도 맞지 않았으면 총알자국 표시를 위해 TraceChannel 을 Visibility 로 트레이스 하여 결과를 추가해준다.
		if (OutHits.Num() == 0)
		{
			FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ GetAvatarActorFromActorInfo());
			TraceParams.bReturnPhysicalMaterial = true;
			TraceParams.bReturnFaceIndex = true;
			GetWorld()->LineTraceSingleByChannel(Impact, InputData.StartTrace, EndTrace, ECC_Visibility, TraceParams);

			// Visibility 채널에 대한 트레이스도 결과가 없다면, 허공에 쐈다는 뜻
			// 이 경우에도 총구 이펙트 등에 방향 정보가 필요하므로, 빈 데이터를 넣어준다.
			if (!Impact.bBlockingHit)
			{
				Impact.Location = EndTrace;
				Impact.ImpactPoint = EndTrace;
			}
			else
			{
#if ENABLE_DRAW_DEBUG
				if (UPFConsoleVariables::DrawBulletHitDuration > 0.0f)
				{
					DrawDebugSphere(GetWorld(), Impact.ImpactPoint, UPFConsoleVariables::DrawBulletHitRadius, 3, FColor::Blue, false, UPFConsoleVariables::DrawBulletHitDuration);
				}
#endif
			}

			OutHits.Add(Impact);
		}
	}
}

void UUPFGameplayAbility_FireWeapon::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	AUPFRangedWeaponInstance* WeaponInstance = GetWeaponInstance();
	if (AvatarPawn && AvatarPawn->IsLocallyControlled() && WeaponInstance)
	{
		FRangedWeaponFiringInput InputData;
		InputData.WeaponInstance = WeaponInstance;
		InputData.bCanPlayBulletFX = (AvatarPawn->GetNetMode() != NM_DedicatedServer);

		//@TODO: Should do more complicated logic here when the player is close to a wall, etc...
		const FTransform TargetTransform = GetTargetingTransform(AvatarPawn, EUPFAbilityTargetingSource::CameraTowardsFocus);
		InputData.AimDir = TargetTransform.GetUnitAxis(EAxis::X);
		InputData.StartTrace = TargetTransform.GetTranslation();
		InputData.EndAim = InputData.StartTrace + InputData.AimDir * WeaponInstance->GetMaxDamageRange();

		TraceBulletsInCartridge(InputData, /*out*/ OutHits);
	}
}

FVector UUPFGameplayAbility_FireWeapon::GetWeaponTargetingSourceLocation() const
{
	// Use Pawn's location as a base
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FVector SourceLoc = AvatarPawn->GetActorLocation();
	const FQuat SourceRot = AvatarPawn->GetActorQuat();

	FVector TargetingSourceLocation = SourceLoc;

	//@TODO: Add an offset from the weapon instance and adjust based on pawn crouch/aiming/etc...

	return TargetingSourceLocation;
}

FTransform UUPFGameplayAbility_FireWeapon::GetTargetingTransform(APawn* SourcePawn, EUPFAbilityTargetingSource Source) const
{
	check(SourcePawn);

	// The caller should determine the transform without calling this if the mode is custom!
	check(Source != EUPFAbilityTargetingSource::Custom);

	const FVector ActorLoc = SourcePawn->GetActorLocation();
	FQuat AimQuat = SourcePawn->GetActorQuat();
	AController* Controller = SourcePawn->Controller;
	FVector SourceLoc;

	double FocalDistance = 1024.0f;
	FVector FocalLoc;

	FVector CamLoc;
	FRotator CamRot;
	bool bFoundFocus = false;


	if ((Controller != nullptr) && ((Source == EUPFAbilityTargetingSource::CameraTowardsFocus) || (Source == EUPFAbilityTargetingSource::PawnTowardsFocus) || (Source == EUPFAbilityTargetingSource::WeaponTowardsFocus)))
	{
		// Get camera position for later
		bFoundFocus = true;

		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC != nullptr)
		{
			PC->GetPlayerViewPoint(/*out*/ CamLoc, /*out*/ CamRot);
		}
		else
		{
			SourceLoc = GetWeaponTargetingSourceLocation();
			CamLoc = SourceLoc;
			CamRot = Controller->GetControlRotation();
		}

		// Determine initial focal point to 
		FVector AimDir = CamRot.Vector().GetSafeNormal();
		FocalLoc = CamLoc + (AimDir * FocalDistance);

		// Move the start and focal point up in front of pawn
		if (PC)
		{
			const FVector WeaponLoc = GetWeaponTargetingSourceLocation();
			CamLoc = FocalLoc + (((WeaponLoc - FocalLoc) | AimDir) * AimDir);
			FocalLoc = CamLoc + (AimDir * FocalDistance);
		}
		//Move the start to be the HeadPosition of the AI
		else if (AAIController* AIController = Cast<AAIController>(Controller))
		{
			CamLoc = SourcePawn->GetActorLocation() + FVector(0, 0, SourcePawn->BaseEyeHeight);
		}

		if (Source == EUPFAbilityTargetingSource::CameraTowardsFocus)
		{
			// If we're camera -> focus then we're done
			return FTransform(CamRot, CamLoc);
		}
	}

	if ((Source == EUPFAbilityTargetingSource::WeaponForward) || (Source == EUPFAbilityTargetingSource::WeaponTowardsFocus))
	{
		SourceLoc = GetWeaponTargetingSourceLocation();
	}
	else
	{
		// Either we want the pawn's location, or we failed to find a camera
		SourceLoc = ActorLoc;
	}

	if (bFoundFocus && ((Source == EUPFAbilityTargetingSource::PawnTowardsFocus) || (Source == EUPFAbilityTargetingSource::WeaponTowardsFocus)))
	{
		// Return a rotator pointing at the focal point from the source
		return FTransform((FocalLoc - SourceLoc).Rotation(), SourceLoc);
	}

	// If we got here, either we don't have a camera or we don't want to use it, either way go forward
	return FTransform(AimQuat, SourceLoc);
}

void UUPFGameplayAbility_FireWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	
	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		UPF_LOG_ABILITY(LogTemp, Log, TEXT("InData Num: %d"), InData.Num());
		
		// 탄 퍼짐 적용
		AUPFRangedWeaponInstance* WeaponInst = GetWeaponInstance();
		check(WeaponInst);
		WeaponInst->AddSpread();

		// 무기에서 발생하는 sfx 및 particle 적용
		WeaponInst->OnFire(InData);

		K2_OnTargetDataReady(InData);
		
		/*
		 * 데미지 GameplayEffect 적용
		 * GameplayCueNotify 를 클라이언트 에서도 실행하기 위해, Authority 여부 분기하지 않고 호출한다.
		 * 실제 Attribute 값 수정은 함수 내부적으로 서버에서만 진행함
		 */
		// ReSharper disable once CppExpressionWithoutSideEffects
		ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
			InData, DamageEffectClass, 1.0f);
		
		SourceASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	}
	else
	{
		UPF_LOG_ABILITY(LogTemp, Error, TEXT("CommitAbility() return false!!"));
		OnFinishWait();
	}
}
