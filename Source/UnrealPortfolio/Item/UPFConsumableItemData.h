// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UPFItemData.h"
#include "UPFConsumableItemData.generated.h"

class AUPFItemInstanceBase;
/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFConsumableItemData : public UUPFItemData
{
	GENERATED_BODY()

public:
	// 이 아이템의 액터 클래스
	UPROPERTY(EditAnywhere)
	TSubclassOf<AUPFItemInstanceBase> InstanceClass;
};
