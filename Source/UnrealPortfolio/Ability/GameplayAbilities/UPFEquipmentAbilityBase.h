// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/UPFGameplayAbility.h"
#include "UPFEquipmentAbilityBase.generated.h"

/**
 * 장비 어빌리티는 장비 Actor가 아닌 캐릭터에 부여된다.
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFEquipmentAbilityBase : public UUPFGameplayAbility
{
	GENERATED_BODY()
	
};
