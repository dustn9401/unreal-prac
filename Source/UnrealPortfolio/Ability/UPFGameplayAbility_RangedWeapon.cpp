// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFGameplayAbility_RangedWeapon.h"

#include "Item/ItemInstance/Equipments/UPFRangedWeaponInstance.h"

UUPFGameplayAbility_RangedWeapon::UUPFGameplayAbility_RangedWeapon()
{
}

AUPFRangedWeaponInstance* UUPFGameplayAbility_RangedWeapon::GetWeaponInstance() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	if (!ensure(Spec)) return nullptr;
	
	AUPFRangedWeaponInstance* WeaponInstance = Cast<AUPFRangedWeaponInstance>(Spec->SourceObject);
	return WeaponInstance;
}
