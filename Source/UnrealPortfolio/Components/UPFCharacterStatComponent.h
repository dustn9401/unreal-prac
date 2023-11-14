// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Attributes/UPFCharacterStatSet.h"
#include "Components/ActorComponent.h"
#include "UPFCharacterStatComponent.generated.h"

class UAbilitySystemComponent;

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

public:
	// Pawn의 PostInitializeComponents 함수에서 호출되는 어트리뷰트 초기화용 함수
	void InitializeAttributes(UAbilitySystemComponent* ASC, FName GroupName, int32 Level);

public:
	TObjectPtr<const UUPFCharacterStatSet> GetBaseStat() const
	{
		return StatSet;
	}

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<const UUPFCharacterStatSet> StatSet;
};
