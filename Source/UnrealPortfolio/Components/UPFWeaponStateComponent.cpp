// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFWeaponStateComponent.h"

// Sets default values for this component's properties
UUPFWeaponStateComponent::UUPFWeaponStateComponent()
{
	
}

void UUPFWeaponStateComponent::AddUnconfirmedServerSideHitMarkers(const FGameplayAbilityTargetDataHandle& InTargetData, const TArray<FHitResult>& FoundHits)
{
}

void UUPFWeaponStateComponent::UpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext)
{
}

double UUPFWeaponStateComponent::GetTimeSinceLastHitNotification() const
{
}

bool UUPFWeaponStateComponent::ShouldShowHitAsSuccess(const FHitResult& Hit) const
{
}

bool UUPFWeaponStateComponent::ShouldUpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext) const
{
}

void UUPFWeaponStateComponent::ActuallyUpdateDamageInstigatedTime()
{
}

void UUPFWeaponStateComponent::ClientConfirmTargetData_Implementation(uint16 UniqueId, bool bSuccess, const TArray<uint8>& HitReplaces)
{
}
