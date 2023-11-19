// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealPortfolio.h"
#include "Ability/UPFGameplayAbility.h"
#include "Engine/DataAsset.h"
#include "UPFCharacterData.generated.h"

/**
 * 캐릭터에게 필요한 데이터들을 캐릭터 클래스에 두지 않고 따로 모아놓기 위한 데이터 클래스
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 이 캐릭터가 보유한 어빌리티들과, 발동시키기 위한 입력 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UUPFAbilitySet> AbilityInputMappingData;
};
