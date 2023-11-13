// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Attributes/UPFCharacterStatSet.h"
#include "Components/ActorComponent.h"
#include "UPFCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHpChangedDelegate, float /*CurrentHp*/, float /*MaxHp*/)

/**
 * 캐릭터의 스텟 컴포넌트
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPORTFOLIO_API UUPFCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUPFCharacterStatComponent();

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<const UUPFCharacterStatSet> BaseStat;	// 캐릭터 자체 스텟

	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<const UUPFCharacterStatSet> ModifierStat;	// 장비, 버프 등의 추가 스텟
};
