// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Attributes/UPFAmmoSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UUPFAmmoSet::UUPFAmmoSet()
{
}

void UUPFAmmoSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;
	
	if (Attr == GetMagazineAmmoAttribute() || Attr == GetExtraAmmoAttribute())
	{
		BroadcastAmmoChangedDelegate();
	}
}

void UUPFAmmoSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UUPFAmmoSet, MagazineAmmo, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUPFAmmoSet, ExtraAmmo, COND_OwnerOnly, REPNOTIFY_Always);
}

void UUPFAmmoSet::OnRep_MagazineAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFAmmoSet, MagazineAmmo, OldValue);

	BroadcastAmmoChangedDelegate();
}

void UUPFAmmoSet::OnRep_ExtraAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUPFAmmoSet, ExtraAmmo, OldValue);

	BroadcastAmmoChangedDelegate();
}

void UUPFAmmoSet::BroadcastAmmoChangedDelegate()
{
	// todo: Throttle 최적화
	OnAmmoChanged.Broadcast(GetMagazineAmmo(), GetExtraAmmo());
}
