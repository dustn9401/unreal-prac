// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/UPFGameplayAbility.h"
#include "UPFGameplayAbility_RangedWeapon.generated.h"

class AUPFRangedWeaponInstance;
/**
 * 원거리 무기가 가지는 어빌리티의 base class
 * 발사, 장전 등으로 파생된다.
 */
UCLASS(Abstract)
class UNREALPORTFOLIO_API UUPFGameplayAbility_RangedWeapon : public UUPFGameplayAbility
{
	GENERATED_BODY()

public:
	UUPFGameplayAbility_RangedWeapon();

protected:
	AUPFRangedWeaponInstance* GetWeaponInstance() const;
};
