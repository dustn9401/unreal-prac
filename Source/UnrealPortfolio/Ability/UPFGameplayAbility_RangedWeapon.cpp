// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFGameplayAbility_RangedWeapon.h"

#include "Character/UPFCharacterBase.h"
#include "Components/UPFCharacterEquipmentComponent.h"
#include "Item/ItemInstance/Equipments/UPFRangedWeaponInstance.h"
#include "Utility/UPFActorUtility.h"

UUPFGameplayAbility_RangedWeapon::UUPFGameplayAbility_RangedWeapon()
{
}

AUPFRangedWeaponInstance* UUPFGameplayAbility_RangedWeapon::GetWeaponInstance() const
{
	AUPFCharacterBase* OwnerCharacter = UPFActorUtility::GetTypedOwnerRecursive<AUPFCharacterBase>(GetOwningActorFromActorInfo());
	if (!ensure(OwnerCharacter)) return nullptr;
	if (!ensure(OwnerCharacter->EquipmentComponent)) return nullptr;

	AUPFRangedWeaponInstance* Ret = Cast<AUPFRangedWeaponInstance>(OwnerCharacter->EquipmentComponent->GetCurrentRangedWeaponInstance());
	return Ret;
}
